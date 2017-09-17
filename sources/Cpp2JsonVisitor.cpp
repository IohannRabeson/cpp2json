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
}

Cpp2JsonVisitor::Cpp2JsonVisitor(Cpp2JsonParameters const& parameters, rapidjson::Document& jsonDocument) :
    m_parameters(parameters),
    m_jsonDocument(jsonDocument),
    m_jsonAllocator(m_jsonDocument.GetAllocator()),
    m_jsonClasses(getOrCreate(m_jsonDocument, "classes", rapidjson::Type::kObjectType).GetObject()),
    m_jsonEnums(getOrCreate(m_jsonDocument, "enums", rapidjson::Type::kObjectType).GetObject())
{
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
    addOrReplaceJsonMember(m_jsonClasses, classDeclaration->getQualifiedNameAsString(), jsonClassObject);
}

void Cpp2JsonVisitor::parseEnum(clang::EnumDecl *enumDeclaration)
{
    auto const integerTypeDeclaration = enumDeclaration->getIntegerType();
    rapidjson::Value jsonEnum(rapidjson::Type::kObjectType);
    rapidjson::Value jsonEnumValues(rapidjson::Type::kObjectType);

    jsonEnum.AddMember("underlyingType", integerTypeDeclaration.getAsString(), m_jsonAllocator);
    for (auto const enumerator : enumDeclaration->enumerators())
    {
        rapidjson::Value jsonEnumeratorValue(enumerator->getNameAsString(), m_jsonAllocator);

        jsonEnumValues.AddMember(jsonEnumeratorValue, enumerator->getInitVal().getExtValue(), m_jsonAllocator);
    }
    jsonEnum.AddMember("values", jsonEnumValues, m_jsonAllocator);
    parseAnnotations(enumDeclaration, jsonEnum);
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
    rapidjson::Value jsonReturnType(returnType.getAsString(), m_jsonAllocator);

    parseFunctionParameters(methodDeclaration, jsonMethodParametersArray);
    jsonMethod.AddMember("name", jsonMethodName, m_jsonAllocator);
    jsonMethod.AddMember("parameters", jsonMethodParameters, m_jsonAllocator);
    jsonMethod.AddMember("result", jsonReturnType, m_jsonAllocator);
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
        auto const parameterType = parameterDeclaration->getType();
        rapidjson::Value jsonParameter(rapidjson::Type::kObjectType);
        rapidjson::Value jsonParamName(parameterDeclaration->getNameAsString(), m_jsonAllocator);
        rapidjson::Value jsonParamType(parameterType.getAsString(), m_jsonAllocator);

        jsonParameter.AddMember("name", jsonParamName, m_jsonAllocator);
        jsonParameter.AddMember("type", jsonParamType, m_jsonAllocator);
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
    auto const fieldType = fieldDeclaration->getType().getLocalUnqualifiedType();
    auto const fieldTypeName = fieldType.getAsString();
    rapidjson::Value jsonFieldObject(rapidjson::Type::kObjectType);

    jsonFieldObject.AddMember("name", fieldName, m_jsonAllocator);
    if (fieldType->isConstantArrayType())
    {
        clang::ConstantArrayType const* const constantArrayType = clang::dyn_cast_or_null<clang::ConstantArrayType const, clang::Type const>(fieldType.getTypePtr());

        // Only 1-dimensional arrays are supported
        // TODO: N-dimensional arrays support (require something like multiple arraySize in JSON?)
        auto const elementType = constantArrayType->getElementType();
        auto const size = constantArrayType->getSize().getLimitedValue();

        jsonFieldObject.AddMember("type", elementType.getAsString(), m_jsonAllocator);
        jsonFieldObject.AddMember("arraySize", size, m_jsonAllocator);
    }
    else
    {
        jsonFieldObject.AddMember("type", fieldTypeName, m_jsonAllocator);
    }
    jsonFieldArray.PushBack(jsonFieldObject, m_jsonAllocator);
}

void Cpp2JsonVisitor::parseBaseClasses(clang::CXXRecordDecl *classDeclaration, rapidjson::Value &jsonClassObject)
{
    if (classDeclaration->getNumBases() > 0u)
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

bool Cpp2JsonVisitor::isExcludedDeclaration(clang::CXXRecordDecl const* declaration) const
{
    return (!isInMainFile(declaration) || hasExcludeAnnotation(declaration));
}

bool Cpp2JsonVisitor::isExcludedDeclaration(clang::CXXMethodDecl const* declaration) const
{
    return (hasExcludeAnnotation(declaration) ||
            declaration->isUserProvided() ||
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
