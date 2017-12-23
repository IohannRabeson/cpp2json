#include "Cpp2JsonVisitor.hpp"
#include "Cpp2JsonParameters.hpp"

#include <clang/Lex/Lexer.h>

#include <iostream>

namespace
{
    static constexpr char const* const ExclusionAnnotationTag = CPP2JSON_EXCLUDE_TAG;
    static constexpr char const* const ArrayAnnotationTag = CPP2JSON_ARRAY_TAG;

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

    struct MatchText
    {
        explicit MatchText(std::string const& text) :
            m_text(text)
        {
        }

        bool operator()(std::string const& other) const
        {
            return m_text == other;
        }

        bool operator()(char const* const other) const
        {
            return m_text == other;
        }
    private:
        std::string const m_text;
    };

    template <class F>
    bool hasAnnotation(clang::Decl const* declaration, F&& f)
    {
        auto attributes = declaration->getAttrs();

        attributes.erase(std::remove_if(attributes.begin(), attributes.end(), &isNotAnAnnotation), attributes.end());
        for (clang::Attr const* attribute : attributes)
        {
            clang::AnnotateAttr const* annotation = clang::cast<clang::AnnotateAttr const, clang::Attr const*>(attribute);

            if (f(annotation->getAnnotation().str()))
            {
                return true;
            }
        }
        return false;
    }

    bool isConstructor(clang::CXXMethodDecl const* methodDeclaration)
    {
        return methodDeclaration && clang::isa<clang::CXXConstructorDecl const>(methodDeclaration);
    }

    bool isDestructor(clang::CXXMethodDecl const* methodDeclaration)
    {
        return methodDeclaration && clang::isa<clang::CXXDestructorDecl const>(methodDeclaration);
    }

    /*!
     *
     * @tparam F Type of functional passed though the method.
     * @param declaration
     * @param selector A predicate use to select an annotation.
     * @param text The annotation text if found, otherwise text is cleared.
     * @return true if an annotation matching the predicate f has been found.
     * @see hasAnnotation()
     */
    bool getAnnotationString(clang::Decl const* declaration, std::function<bool(std::string const&)>&& selector, std::string& text)
    {
        auto attributes = declaration->getAttrs();

        // Remove from attributes all attribute which are not an annotation.
        attributes.erase(std::remove_if(attributes.begin(), attributes.end(), &isNotAnAnnotation), attributes.end());
        for (clang::Attr const* attribute : attributes)
        {
            clang::AnnotateAttr const* annotation = clang::cast<clang::AnnotateAttr const, clang::Attr const*>(attribute);
            auto const annotationText = annotation->getAnnotation().str();

            if (selector(annotationText))
            {
                text = annotationText;
                return true;
            }
        }
        text.clear();
        return false;
    }

    /*!
     * \brief Clean a qualified type.
     * \return Returns the qualified type passed as parameter with qualifiers array, pointer or reference removed.
     * Example:
     *  - int const => int const
     *  - int [123] => int
     *  - int const [123] => int const
     * \param type
     */
    clang::QualType getCleanedType(clang::QualType const type)
    {
        auto other = type;

        if (other->isPointerType())
        {
            other = other->getPointeeType();
        }
        // TODO: maybe useless? Is using the same code as other->isPointerType() sufficient?
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

    void removeAll(std::string& str, std::string const& toRemove)
    {
        auto pos = str.find(toRemove);

        while (pos != std::string::npos)
        {
            str.erase(pos, toRemove.size());
            pos = str.find(toRemove);
        }
    }

    /*!
     * Add or get a JSON node.
     * @param document JSON document
     * @param key Key of the node
     * @param type Type of the requested node
     * @return reference on the node whether added or found
     */
    rapidjson::Value& getOrCreate(rapidjson::Document& document, std::string const& key, rapidjson::Type const type)
    {
        assert(document.IsObject());

        if (!document.HasMember(key))
        {
            rapidjson::Value jsonKey(key, document.GetAllocator());
            rapidjson::Value newArray(type);

            document.AddMember(jsonKey, newArray, document.GetAllocator());
        }
        return document[key];
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

    // TODO: store "cpp2json_format" into a variable.
    rapidjson::Value& formatVersion = getOrCreate(jsonDocument, "cpp2json_format", rapidjson::Type::kStringType);

    formatVersion.SetString(CPP2JSON_CORE_VERSION);
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

    // List enum's values
    parseEnumerationValues(enumDeclaration, jsonEnum, jsonEnumValues);

    parseAnnotations(enumDeclaration, jsonEnum);
    parseIncludeDeclaration(enumDeclaration, jsonEnum);
    addOrReplaceJsonMember(m_jsonEnums, enumDeclaration->getQualifiedNameAsString(), jsonEnum);
}

void Cpp2JsonVisitor::parseEnumerationValues(clang::EnumDecl const* enumDeclaration, rapidjson::Value& jsonEnum, rapidjson::Value& jsonEnumValues) const
{
    for (auto const enumerator : enumDeclaration->enumerators())
    {
        rapidjson::Value jsonEnumeratorValue(enumerator->getNameAsString(), m_jsonAllocator);

        jsonEnumValues.AddMember(jsonEnumeratorValue, enumerator->getInitVal().getExtValue(), m_jsonAllocator);
    }

    jsonEnum.AddMember("values", jsonEnumValues, m_jsonAllocator);
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
    parseType(fieldDeclaration, jsonFieldObject, "type", fieldDeclaration->getASTContext());
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


void Cpp2JsonVisitor::parseType(clang::ValueDecl const* const valueDeclaration, rapidjson::Value& root, std::string const& jsonKey, clang::ASTContext& context) const
{
    auto const valueType = valueDeclaration->getType();
    std::string arraySizeExpression;
    std::string arraySizeAnnotationText;

    if (valueType->isConstantArrayType())
    {
        clang::ConstantArrayType const* const constantArrayType = clang::dyn_cast_or_null<clang::ConstantArrayType const, clang::Type const>(valueType.getTypePtr());
        auto const size = constantArrayType->getSize().getLimitedValue();

        arraySizeExpression = std::to_string(size);
    }
    else if (getAnnotationString(valueDeclaration, [](std::string const& annotationText)
    {
        return annotationText.find("cpp2json_array") == 0;
    }, arraySizeAnnotationText))
    {
        // Extract the expression size.
        // Always between `"`.
        auto endPos = arraySizeAnnotationText.find_last_of('\"');
        int beginPos = 0;

        // Ill-formed annotation text (missing trailing \")
        if (endPos != std::string::npos)
        {
            auto i = endPos - 1;

            while (i > 0 && i < arraySizeAnnotationText.size())
            {
                if (arraySizeAnnotationText[i] == '\"')
                {
                    beginPos = i + 1;
                    break;
                }
                --i;
            }
        }
        arraySizeExpression = arraySizeAnnotationText.substr(beginPos, (endPos - beginPos));
    }

    parseType(valueType, root, jsonKey, context, arraySizeExpression);
}

void Cpp2JsonVisitor::parseType(clang::QualType const& completeType,
                                rapidjson::Value& root,
                                std::string const& jsonKey,
                                clang::ASTContext& context,
                                std::string const& dynamicArraySizeExpression) const
{
    clang::QualType effectiveType = completeType;
    rapidjson::Value jsonTypeObject(rapidjson::Type::kObjectType);

    // NOTE: array size is an expression and is stored by a string.
    if (std::count_if(dynamicArraySizeExpression.begin(), dynamicArraySizeExpression.end(),
                      [](char const c){ return !std::isspace(c); }) > 0)
    {
        rapidjson::Value sizeExpression(dynamicArraySizeExpression.c_str(), m_jsonAllocator);

        jsonTypeObject.AddMember("array_size", sizeExpression, m_jsonAllocator);
    }

    if (completeType->isArrayType())
    {
        clang::ArrayType const* const arrayType = clang::dyn_cast_or_null<clang::ArrayType const, clang::Type const>(completeType.getTypePtr());

        assert (arrayType);

        effectiveType = arrayType->getElementType();
    }
    else if (completeType->isConstantArrayType())
    {
        clang::ConstantArrayType const* const arrayType = clang::dyn_cast_or_null<clang::ConstantArrayType const, clang::Type const>(completeType.getTypePtr());

        assert (arrayType);

        effectiveType = arrayType->getElementType();
    }

    // \var cleanedType Qualified type without any array marker, or pointer or reference.
    clang::QualType const cleanedType = getCleanedType(effectiveType);

    jsonTypeObject.AddMember("key", getCleanedTypeString(effectiveType), m_jsonAllocator);
    jsonTypeObject.AddMember("expression", getNormalizedTypeString(completeType), m_jsonAllocator);
    jsonTypeObject.AddMember("pointer", effectiveType->isPointerType(), m_jsonAllocator);
    jsonTypeObject.AddMember("array", completeType->isArrayType(), m_jsonAllocator);
    jsonTypeObject.AddMember("reference", effectiveType->isReferenceType(), m_jsonAllocator);
    jsonTypeObject.AddMember("const", cleanedType.isLocalConstQualified(), m_jsonAllocator);
    jsonTypeObject.AddMember("volatile", effectiveType.isVolatileQualified(), m_jsonAllocator);
    jsonTypeObject.AddMember("literal", cleanedType->isLiteralType(context), m_jsonAllocator);
    jsonTypeObject.AddMember("enum", cleanedType->isEnumeralType(), m_jsonAllocator);
    root.AddMember(rapidjson::Value(jsonKey, m_jsonAllocator), jsonTypeObject, m_jsonAllocator);
}

bool Cpp2JsonVisitor::isExcludedDeclaration(clang::CXXRecordDecl const* declaration) const
{
    return (!isInMainFile(declaration) || hasExcludeAnnotation(declaration));
}

bool Cpp2JsonVisitor::isExcludedDeclaration(clang::CXXMethodDecl const* declaration) const
{
    return (hasExcludeAnnotation(declaration) ||
            !declaration->isUserProvided() || // isUserProvided ? I don't remember what that means. Guess this is true if the C++ come from the current file.
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
    return hasAnnotation(declaration, MatchText(m_parameters.excludeAnnotationContent)) ||
            hasAnnotation(declaration, MatchText(ExclusionAnnotationTag));
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
    static std::vector<std::string> const ToRemove =
    {
        "class ",
        "struct ",
        "enum "
    };

    for (auto const& toRemove : ToRemove)
    {
        removeAll(typeName, toRemove);
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