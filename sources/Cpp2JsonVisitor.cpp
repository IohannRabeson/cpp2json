#include "Cpp2JsonVisitor.hpp"
#include "Cpp2JsonParameters.hpp"

#include <iostream>

namespace
{
    bool isInMainFile(clang::Decl const* declaration)
    {
        auto const& sourceManager = declaration->getASTContext().getSourceManager();

        return sourceManager.isInMainFile(declaration->getLocation());
    }

    bool isClassOrStruct(clang::CXXRecordDecl const* declaration)
    {
        return declaration->isClass() || declaration->isStruct();
    }

    bool isNotAnAnnotation(clang::Attr* const attribute)
    {
        return !clang::isa<clang::AnnotateAttr, clang::Attr*>(attribute);
    }

    bool hasAnnotation(clang::Decl const* declaration, std::string const& annotationText)
    {
        auto attributes = declaration->getAttrs();

        attributes.erase(std::remove_if(attributes.begin(), attributes.end(), &isNotAnAnnotation), attributes.end());
        for (clang::Attr const* attribute : attributes)
        {
            clang::AnnotateAttr const* annotation = clang::cast<clang::AnnotateAttr const, clang::Attr const*>(attribute);

            if (annotation->getAnnotation().str() == annotationText)
            {
                return true;
            }
        }
        return false;
    }

    bool isConstructor(clang::CXXMethodDecl const* methodDeclaration)
    {
        return clang::isa<clang::CXXConstructorDecl const>(methodDeclaration);
    }

    bool isDestructor(clang::CXXMethodDecl const* methodDeclaration)
    {
        return clang::isa<clang::CXXDestructorDecl const>(methodDeclaration);
    }

    rapidjson::Value& getOrCreate(rapidjson::Document& document, std::string const& key, rapidjson::Type const type)
    {
        if (!document.HasMember(key))
        {
            rapidjson::Value jsonKey(key, document.GetAllocator());
            rapidjson::Value newArray(type);

            document.AddMember(jsonKey, newArray, document.GetAllocator());
        }
        return document[key];
    }

    std::string const& getJsonBoolean(bool const value)
    {
        static std::string const TrueString = "true";
        static std::string const FalseString = "false";

        return value ? TrueString : FalseString;
    }

    /*!
     * \brief Returns the qualified type without array or pointer or reference informations.
     *
     * Example:
     *  - int const => int const
     *  - int [123] => int
     *  - int const [123] => int const
     * \param type
     * \return
     */
    clang::QualType getCleanedType(clang::QualType const type)
    {
        auto other = type;

        if (other->isPointerType())
        {
            other = other->getPointeeType();
        }
        // TODO: maybe useless? Maybe using the same code as other->isPointerType() is sufficient?
        else if (type->isReferenceType())
        {
            auto const* referenceType = clang::dyn_cast<clang::ReferenceType>(type);

            other = referenceType->getPointeeType();
        }
        else if (type->isArrayType())
        {
            auto const* arrayType = type->getAsArrayTypeUnsafe();

            other = arrayType->getElementType();
        }
        return other;
    }

    std::string replaceAll(std::string const& str, std::string const& toReplace, std::string const& replacement)
    {
        std::string result = str;
        auto pos = result.find(toReplace);

        while (pos != std::string::npos)
        {
            result.replace(pos, toReplace.size(), replacement);
            pos = result.find(toReplace, pos + replacement.size());
        }
        return result;
    }
}

Cpp2JsonVisitor::Cpp2JsonVisitor(Cpp2JsonParameters const& parameters, rapidjson::Document& jsonDocument) :
    m_parameters(parameters),
    m_jsonDocument(jsonDocument),
    m_jsonAllocator(m_jsonDocument.GetAllocator()),
    m_jsonClasses(getOrCreate(m_jsonDocument, "classes", rapidjson::Type::kObjectType).GetObject()),
    m_jsonEnums(getOrCreate(m_jsonDocument, "enums", rapidjson::Type::kObjectType).GetObject())
{
    addTypeNameReplacer("_Bool", "bool");
}

void Cpp2JsonVisitor::addTypeNameReplacer(std::string const& original, std::string const& replacement)
{
    m_typeNameReplacer.emplace(original, replacement);
}

bool Cpp2JsonVisitor::VisitEnumDecl(clang::EnumDecl *declaration)
{
    if (declaration->isCompleteDefinition() &&
        !isExcludedDeclaration(declaration))
    {
        parseEnum(declaration);
    }
    return true;
}

bool Cpp2JsonVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *declaration)
{
    if (declaration->hasDefinition() &&
        declaration->isCompleteDefinition() &&
        !isExcludedDeclaration(declaration) &&
        isClassOrStruct(declaration))
    {
        parseClass(declaration);
    }
    return true;
}

void Cpp2JsonVisitor::parseClass(clang::CXXRecordDecl *classDeclaration)
{
    rapidjson::Value jsonClassObject(rapidjson::Type::kObjectType);

    parseFields(classDeclaration, jsonClassObject);
    parseMethods(classDeclaration, jsonClassObject);
    parseAnnotations(classDeclaration, jsonClassObject);
    parseClassTemplateParameters(classDeclaration, jsonClassObject);
    parseBaseClasses(classDeclaration, jsonClassObject);
    parseIncludeDeclaration(classDeclaration, jsonClassObject);
    addOrReplaceJsonMember(m_jsonClasses, classDeclaration->getQualifiedNameAsString(), jsonClassObject);
}

void Cpp2JsonVisitor::parseEnum(clang::EnumDecl *enumDeclaration)
{
    auto const integerTypeDeclaration = enumDeclaration->getIntegerType();
    rapidjson::Value jsonEnum(rapidjson::Type::kObjectType);
    rapidjson::Value jsonEnumValues(rapidjson::Type::kObjectType);

    jsonEnum.AddMember("underlying_type", getCleanedTypeString(integerTypeDeclaration), m_jsonAllocator);
    for (auto const enumerator : enumDeclaration->enumerators())
    {
        rapidjson::Value jsonEnumeratorValue(enumerator->getNameAsString(), m_jsonAllocator);

        jsonEnumValues.AddMember(jsonEnumeratorValue, enumerator->getInitVal().getExtValue(), m_jsonAllocator);
    }
    jsonEnum.AddMember("values", jsonEnumValues, m_jsonAllocator);
    parseAnnotations(enumDeclaration, jsonEnum);
    parseIncludeDeclaration(enumDeclaration, jsonEnum);
    addOrReplaceJsonMember(m_jsonEnums, enumDeclaration->getQualifiedNameAsString(), jsonEnum);
}

void Cpp2JsonVisitor::parseAnnotations(clang::Decl const* classDeclaration, rapidjson::Value& jsonObject)
{
    rapidjson::Value jsonAnnotationArray(rapidjson::Type::kArrayType);
    auto attributes = classDeclaration->getAttrs();

    for (clang::Attr const* attribute : attributes)
    {
        clang::AnnotateAttr const* annotation = clang::cast<clang::AnnotateAttr const, clang::Attr const*>(attribute);
        rapidjson::Value jsonAnnotation(annotation->getAnnotation().str(), m_jsonAllocator);

        jsonAnnotationArray.PushBack(jsonAnnotation, m_jsonAllocator);
    }
    jsonObject.AddMember("annotations", jsonAnnotationArray, m_jsonAllocator);
}

void Cpp2JsonVisitor::parseMethod(clang::CXXMethodDecl const *methodDeclaration, rapidjson::Value::Array &jsonMethodArray)
{
    auto const returnType = methodDeclaration->getReturnType();
    rapidjson::Value jsonMethod(rapidjson::Type::kObjectType);
    rapidjson::Value jsonMethodName(methodDeclaration->getNameAsString(), m_jsonAllocator);
    rapidjson::Value jsonMethodParameters(rapidjson::Type::kArrayType);
    rapidjson::Value::Array jsonMethodParametersArray = jsonMethodParameters.GetArray();

    parseFunctionParameters(methodDeclaration, jsonMethodParametersArray);
    jsonMethod.AddMember("name", jsonMethodName, m_jsonAllocator);
    jsonMethod.AddMember("parameters", jsonMethodParameters, m_jsonAllocator);
    parseType(returnType, jsonMethod, "result_type", methodDeclaration->getASTContext());
    jsonMethodArray.PushBack(jsonMethod, m_jsonAllocator);
}

void Cpp2JsonVisitor::parseMethods(clang::CXXRecordDecl *classDeclaration, rapidjson::Value &jsonClassObject)
{
    rapidjson::Value jsonMethod(rapidjson::Type::kArrayType);
    rapidjson::Value::Array jsonMethodArray = jsonMethod.GetArray();

    for (clang::CXXMethodDecl const* methodDeclaration : classDeclaration->methods())
    {
        if (!isExcludedDeclaration(methodDeclaration))
        {
            parseMethod(methodDeclaration, jsonMethodArray);
        }
    }
    jsonClassObject.AddMember("methods", jsonMethod, m_jsonAllocator);
}

void Cpp2JsonVisitor::parseFunctionParameters(clang::FunctionDecl const* functionDecl, rapidjson::Value::Array& jsonParameterArray)
{
    for (clang::ParmVarDecl const* parameterDeclaration : functionDecl->parameters())
    {
        clang::QualType const parameterType = parameterDeclaration->getType();
        rapidjson::Value jsonParameter(rapidjson::Type::kObjectType);
        rapidjson::Value jsonParamName(parameterDeclaration->getNameAsString(), m_jsonAllocator);

        jsonParameter.AddMember("name", jsonParamName, m_jsonAllocator);
        parseType(parameterType, jsonParameter, "paramType", functionDecl->getASTContext());
        jsonParameterArray.PushBack(jsonParameter, m_jsonAllocator);
    }
}

void Cpp2JsonVisitor::parseFields(clang::CXXRecordDecl *classDeclaration, rapidjson::Value& jsonClassObject)
{
    rapidjson::Value jsonFieldArray(rapidjson::Type::kArrayType);

    for (auto fieldIt = classDeclaration->field_begin(); fieldIt != classDeclaration->field_end(); ++fieldIt)
    {
        clang::FieldDecl* const fieldDeclaration = *fieldIt;

        if (!hasExcludeAnnotation(fieldDeclaration))
        {
            parseField(fieldDeclaration, jsonFieldArray);
        }
    }
    jsonClassObject.AddMember("fields", jsonFieldArray, m_jsonAllocator);
}

void Cpp2JsonVisitor::parseField(clang::FieldDecl *fieldDeclaration, rapidjson::Value& jsonFieldArray)
{
    auto const fieldName = fieldDeclaration->getNameAsString();
    rapidjson::Value jsonFieldObject(rapidjson::Type::kObjectType);

    jsonFieldObject.AddMember("name", fieldName, m_jsonAllocator);
    parseType(fieldDeclaration->getType(), jsonFieldObject, "type", fieldDeclaration->getASTContext());
    jsonFieldArray.PushBack(jsonFieldObject, m_jsonAllocator);
}

void Cpp2JsonVisitor::parseBaseClasses(clang::CXXRecordDecl *classDeclaration, rapidjson::Value &jsonClassObject)
{
    rapidjson::Value jsonBaseClassArray(rapidjson::Type::kArrayType);

    for (auto baseIt = classDeclaration->bases_begin(); baseIt != classDeclaration->bases_end(); ++baseIt)
    {
        clang::CXXBaseSpecifier const baseSpecifier = *baseIt;
        auto const* baseDecl = baseSpecifier.getType()->getAsCXXRecordDecl();
        auto const baseClassName = baseDecl->getQualifiedNameAsString();
        rapidjson::Value jsonBaseClassName(baseClassName, m_jsonAllocator);

        jsonBaseClassArray.PushBack(jsonBaseClassName, m_jsonAllocator);
    }
    jsonClassObject.AddMember("bases", jsonBaseClassArray, m_jsonAllocator);

}

void Cpp2JsonVisitor::parseClassTemplateParameters(clang::CXXRecordDecl *classDeclaration, rapidjson::Value &jsonClassObject)
{
    clang::ClassTemplateDecl* const templateDecl = classDeclaration->getDescribedClassTemplate();

    if (templateDecl)
    {
        auto const* const templateParameters = templateDecl->getTemplateParameters();

        if (templateParameters->size() > 0)
        {
            rapidjson::Value jsonTemplateParameters(rapidjson::Type::kArrayType);

            for (auto const* parameter : *templateParameters)
            {
                rapidjson::Value jsonTemplateParameterName(parameter->getNameAsString(), m_jsonAllocator);

                jsonTemplateParameters.PushBack(jsonTemplateParameterName, m_jsonAllocator);
            }
            jsonClassObject.AddMember("templateParameters", jsonTemplateParameters, m_jsonAllocator);
        }
    }
}

void Cpp2JsonVisitor::parseIncludeDeclaration(clang::Decl* declaration, rapidjson::Value& jsonObject)
{
    auto const& sourceManager = declaration->getASTContext().getSourceManager();
    auto const location = declaration->getLocation();
    std::string const fileName = sourceManager.getFilename(location);

    jsonObject.AddMember("file", fileName, m_jsonAllocator);
}

void Cpp2JsonVisitor::parseType(clang::QualType const& type, rapidjson::Value& root, std::string const& jsonKey, clang::ASTContext& context) const
{
    // \var cleanedType Qualified type without array marker, or pointer, reference.
    auto const cleanedType = getCleanedType(type);
    rapidjson::Value jsonTypeObject(rapidjson::Type::kObjectType);

    // The size array is an informatioon only available if the array is a constant array.
    // If it is an another array type (see derived classes from clang::ArrayType) then the user
    // should use annotations to define the array size.
    // This should handle any possible cases (even the cases where a variable stores the array size).
    if (type->isConstantArrayType())
    {
        clang::ConstantArrayType const* const constantArrayType = clang::dyn_cast_or_null<clang::ConstantArrayType const, clang::Type const>(type.getTypePtr());

        // Only 1-dimensional arrays are supported
        // TODO: N-dimensional arrays support (require something like multiple arraySize in JSON?)
        auto const size = constantArrayType->getSize().getLimitedValue();

        jsonTypeObject.AddMember("key", getCleanedTypeString(cleanedType), m_jsonAllocator);
        jsonTypeObject.AddMember("expression", getNormalizedTypeString(type), m_jsonAllocator);
        jsonTypeObject.AddMember("array_size", size, m_jsonAllocator);
    }
    else
    {
        jsonTypeObject.AddMember("key", getCleanedTypeString(type), m_jsonAllocator);
        jsonTypeObject.AddMember("expression", getNormalizedTypeString(type), m_jsonAllocator);
    }
    jsonTypeObject.AddMember("pointer", getJsonBoolean(type->isPointerType()), m_jsonAllocator);
    jsonTypeObject.AddMember("array", getJsonBoolean(type->isArrayType()), m_jsonAllocator);
    jsonTypeObject.AddMember("reference", getJsonBoolean(type->isReferenceType()), m_jsonAllocator);
    jsonTypeObject.AddMember("const", getJsonBoolean(cleanedType.isLocalConstQualified()), m_jsonAllocator);
    jsonTypeObject.AddMember("volatile", getJsonBoolean(type.isVolatileQualified()), m_jsonAllocator);
    jsonTypeObject.AddMember("literal", getJsonBoolean(cleanedType->isLiteralType(context)), m_jsonAllocator);
    jsonTypeObject.AddMember("enum", getJsonBoolean(cleanedType->isEnumeralType()), m_jsonAllocator);
    root.AddMember(rapidjson::Value(jsonKey, m_jsonAllocator), jsonTypeObject, m_jsonAllocator);
}

bool Cpp2JsonVisitor::isExcludedDeclaration(clang::CXXRecordDecl const* declaration) const
{
    return (!isInMainFile(declaration) || hasExcludeAnnotation(declaration));
}

bool Cpp2JsonVisitor::isExcludedDeclaration(clang::CXXMethodDecl const* declaration) const
{
    return (hasExcludeAnnotation(declaration) ||
            !declaration->isUserProvided() || // isUserProvided ? I don't remember what that means.
            declaration->isCopyAssignmentOperator() ||
            declaration->isMoveAssignmentOperator() ||
            isConstructor(declaration) ||
            isDestructor(declaration) ||
            declaration->isStatic());
}

bool Cpp2JsonVisitor::isExcludedDeclaration(clang::EnumDecl const* declaration) const
{
    return !isInMainFile(declaration) || hasExcludeAnnotation(declaration);
}

bool Cpp2JsonVisitor::hasExcludeAnnotation(clang::Decl const* declaration) const
{
    return hasAnnotation(declaration, m_parameters.excludeAnnotationContent);
}

void Cpp2JsonVisitor::addOrReplaceJsonMember(rapidjson::Value::Object &object, const std::string &key, rapidjson::Value& value)
{
    auto memberIt = object.FindMember(key);

    if (memberIt != object.MemberEnd())
    {
        object.EraseMember(memberIt);
    }
    object.AddMember(rapidjson::Value(key, m_jsonAllocator), value, m_jsonAllocator);
}

std::string Cpp2JsonVisitor::getCleanedTypeString(const clang::QualType &type) const
{
    auto const effectiveType = getCleanedType(type).getUnqualifiedType();
    std::string typeName = getNormalizedTypeString(effectiveType);

    return typeName;
}

std::string Cpp2JsonVisitor::replaceTypeNames(std::string typeName) const
{
    for (auto const& replacement : m_typeNameReplacer)
    {
        typeName = replaceAll(typeName, replacement.first, replacement.second);
    }
    return typeName;
}

std::string Cpp2JsonVisitor::removeClassStructEnum(std::string typeName) const
{
    static std::vector<std::string> const KeywordToRemove =
    {
        "class ",
        "struct ",
        "enum "
    };

    for (auto const& toRemove : KeywordToRemove)
    {
        // TODO: extract this code => function std::string removeAll(std::string, std::string)
        auto pos = typeName.find(toRemove);

        while (pos != std::string::npos)
        {
            typeName.erase(pos, toRemove.size());
            pos = typeName.find(toRemove);
        }
    }
    return typeName;
}

std::string Cpp2JsonVisitor::getNormalizedTypeString(const clang::QualType& qualType) const
{
    std::string nameAsString = qualType.getAsString();

    nameAsString = removeClassStructEnum(nameAsString);
    nameAsString = replaceTypeNames(nameAsString);
    return nameAsString;
}
