#include "AnimatedModel.h"
#include "CommonValues.h"

#include <iostream>
#include <algorithm>
#include <cctype>

static std::string GetDirectoryFromPath(const std::string& path)
{
    size_t slash = path.find_last_of("/\\");
    if (slash == std::string::npos)
        return ".";
    return path.substr(0, slash);
}

static std::string GetFileNameFromPath(const std::string& path)
{
    size_t slash = path.find_last_of("/\\");
    if (slash == std::string::npos)
        return path;
    return path.substr(slash + 1);
}

static std::string ToLowerText(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return value;
}

AnimatedBone::AnimatedBone(const std::string& boneName, int boneID, const aiNodeAnim* channel)
{
    name = boneName;
    id = boneID;
    localTransform = glm::mat4(1.0f);

    numPositions = channel->mNumPositionKeys;
    for (int positionIndex = 0; positionIndex < numPositions; ++positionIndex)
    {
        KeyPosition data;
        data.position = glm::vec3(channel->mPositionKeys[positionIndex].mValue.x,
            channel->mPositionKeys[positionIndex].mValue.y,
            channel->mPositionKeys[positionIndex].mValue.z);
        data.timeStamp = (float)channel->mPositionKeys[positionIndex].mTime;
        positions.push_back(data);
    }

    numRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < numRotations; ++rotationIndex)
    {
        KeyRotation data;
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        data.orientation = glm::normalize(glm::quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z));
        data.timeStamp = (float)channel->mRotationKeys[rotationIndex].mTime;
        rotations.push_back(data);
    }

    numScalings = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < numScalings; ++keyIndex)
    {
        KeyScale data;
        data.scale = glm::vec3(channel->mScalingKeys[keyIndex].mValue.x,
            channel->mScalingKeys[keyIndex].mValue.y,
            channel->mScalingKeys[keyIndex].mValue.z);
        data.timeStamp = (float)channel->mScalingKeys[keyIndex].mTime;
        scales.push_back(data);
    }
}

void AnimatedBone::Update(float animationTime)
{
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    localTransform = translation * rotation * scale;
}

float AnimatedBone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    if (framesDiff == 0.0f)
        return 0.0f;
    return midWayLength / framesDiff;
}

int AnimatedBone::GetPositionIndex(float animationTime)
{
    for (int index = 0; index < numPositions - 1; ++index)
    {
        if (animationTime < positions[index + 1].timeStamp)
            return index;
    }
    return std::max(0, numPositions - 2);
}

int AnimatedBone::GetRotationIndex(float animationTime)
{
    for (int index = 0; index < numRotations - 1; ++index)
    {
        if (animationTime < rotations[index + 1].timeStamp)
            return index;
    }
    return std::max(0, numRotations - 2);
}

int AnimatedBone::GetScaleIndex(float animationTime)
{
    for (int index = 0; index < numScalings - 1; ++index)
    {
        if (animationTime < scales[index + 1].timeStamp)
            return index;
    }
    return std::max(0, numScalings - 2);
}

glm::mat4 AnimatedBone::InterpolatePosition(float animationTime)
{
    if (numPositions == 0)
        return glm::mat4(1.0f);
    if (numPositions == 1)
        return glm::translate(glm::mat4(1.0f), positions[0].position);

    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 AnimatedBone::InterpolateRotation(float animationTime)
{
    if (numRotations == 0)
        return glm::mat4(1.0f);
    if (numRotations == 1)
        return glm::toMat4(glm::normalize(rotations[0].orientation));

    int p0Index = GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(rotations[p0Index].timeStamp, rotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(rotations[p0Index].orientation, rotations[p1Index].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
}

glm::mat4 AnimatedBone::InterpolateScaling(float animationTime)
{
    if (numScalings == 0)
        return glm::mat4(1.0f);
    if (numScalings == 1)
        return glm::scale(glm::mat4(1.0f), scales[0].scale);

    int p0Index = GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(scales[p0Index].timeStamp, scales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}

AnimatedMesh::AnimatedMesh()
{
}

void AnimatedMesh::CreateMesh(const std::vector<AnimatedVertex>& vertices, const std::vector<unsigned int>& indices, unsigned int matIndex)
{
    materialIndex = matIndex;
    indexCount = (GLsizei)indices.size();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(AnimatedVertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, TexCoords));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Normal));

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, BoneIDs));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Weights));

    glBindVertexArray(0);
}

void AnimatedMesh::RenderMesh()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void AnimatedMesh::ClearMesh()
{
    if (IBO != 0)
    {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }
    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    indexCount = 0;
}

AnimatedMesh::~AnimatedMesh()
{
    ClearMesh();
}

AnimatedModel::AnimatedModel()
{
    finalBoneMatrices.reserve(MAX_ANIMATED_BONES);
    for (int i = 0; i < MAX_ANIMATED_BONES; i++)
        finalBoneMatrices.push_back(glm::mat4(1.0f));
}

void AnimatedModel::LoadModel(const std::string& fileName)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fileName,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_LimitBoneWeights |
        aiProcess_ImproveCacheLocality);

    if (!scene || !scene->mRootNode)
    {
        printf("Fallo en cargar modelo animado: %s\n%s\n", fileName.c_str(), importer.GetErrorString());
        loaded = false;
        return;
    }

    std::string modelDirectory = GetDirectoryFromPath(fileName);
    whiteTextureID = CreateWhiteTexture();

    globalInverseTransform = glm::inverse(ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation));
    LoadNode(scene->mRootNode, scene);
    LoadMaterials(scene, modelDirectory);
    ReadHeirarchyData(rootNode, scene->mRootNode);

    if (scene->mNumAnimations > 0)
    {
        const aiAnimation* animation = scene->mAnimations[0];
        duration = (float)animation->mDuration;
        ticksPerSecond = animation->mTicksPerSecond != 0 ? (int)animation->mTicksPerSecond : 25;
        ReadMissingBones(animation);
        printf("Modelo animado cargado: %s | animacion: %s | huesos: %d\n",
            fileName.c_str(), animation->mName.C_Str(), boneCounter);
    }
    else
    {
        printf("El modelo %s cargo, pero no trae animaciones.\n", fileName.c_str());
    }

    loaded = true;
}

void AnimatedModel::LoadNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
        LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        LoadNode(node->mChildren[i], scene);
}

void AnimatedModel::LoadMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<AnimatedVertex> vertices;
    std::vector<unsigned int> indices;

    vertices.resize(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        AnimatedVertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->mTextureCoords[0])
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        if (mesh->HasNormals())
            vertex.Normal = glm::vec3(-mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z);
        else
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);

        vertices[i] = vertex;
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    ExtractBoneWeightForVertices(vertices, mesh, scene);

    AnimatedMesh* newMesh = new AnimatedMesh();
    newMesh->CreateMesh(vertices, indices, mesh->mMaterialIndex);
    meshList.push_back(newMesh);
}

void AnimatedModel::SetVertexBoneData(AnimatedVertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.BoneIDs[i] < 0)
        {
            vertex.Weights[i] = weight;
            vertex.BoneIDs[i] = boneID;
            return;
        }
    }
}

void AnimatedModel::ExtractBoneWeightForVertices(std::vector<AnimatedVertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            if (boneCounter >= MAX_ANIMATED_BONES)
                continue;

            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCounter;
            newBoneInfo.offset = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCounter;
            boneCounter++;
        }
        else
        {
            boneID = boneInfoMap[boneName].id;
        }

        aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
        unsigned int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (unsigned int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            unsigned int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            if (vertexId < vertices.size())
                SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

void AnimatedModel::LoadMaterials(const aiScene* scene, const std::string& modelDirectory)
{
    textureIDList.resize(scene->mNumMaterials, whiteTextureID);

    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* material = scene->mMaterials[i];
        GLuint tex = LoadTextureFromMaterial(material, scene, modelDirectory);
        if (tex != 0)
            textureIDList[i] = tex;
    }
}

GLuint AnimatedModel::LoadTextureFromMaterial(aiMaterial* material, const aiScene* scene, const std::string& modelDirectory)
{
    aiString path;
    bool hasTexture = false;

    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        hasTexture = true;
    else if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &path) == AI_SUCCESS)
        hasTexture = true;

    if (!hasTexture)
        return whiteTextureID;

    std::string texturePath = path.C_Str();
    if (texturePath.size() > 0 && texturePath[0] == '*')
    {
        int textureIndex = atoi(texturePath.c_str() + 1);
        if (textureIndex >= 0 && textureIndex < (int)scene->mNumTextures)
            return LoadEmbeddedTexture(scene->mTextures[textureIndex]);
    }

    std::string fileName = GetFileNameFromPath(texturePath);
    GLuint tex = LoadTextureFromFilePath(modelDirectory + "/" + fileName);
    if (tex != 0) return tex;

    tex = LoadTextureFromFilePath(std::string("Textures/") + fileName);
    if (tex != 0) return tex;

    tex = LoadTextureFromFilePath(texturePath);
    if (tex != 0) return tex;

    printf("No se pudo cargar textura del modelo animado: %s\n", texturePath.c_str());
    return whiteTextureID;
}

GLuint AnimatedModel::LoadTextureFromFilePath(const std::string& path)
{
    int width = 0, height = 0, bitDepth = 0;
    unsigned char* texData = stbi_load(path.c_str(), &width, &height, &bitDepth, 0);
    if (!texData)
        return 0;

    GLenum format = GL_RGB;
    if (bitDepth == 1) format = GL_RED;
    else if (bitDepth == 3) format = GL_RGB;
    else if (bitDepth == 4) format = GL_RGBA;

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(texData);
    return textureID;
}

GLuint AnimatedModel::LoadEmbeddedTexture(const aiTexture* texture)
{
    if (!texture)
        return 0;

    int width = 0, height = 0, bitDepth = 0;
    unsigned char* texData = nullptr;

    if (texture->mHeight == 0)
    {
        texData = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &bitDepth, 0);
    }
    else
    {
        width = texture->mWidth;
        height = texture->mHeight;
        bitDepth = 4;
        texData = reinterpret_cast<unsigned char*>(texture->pcData);
    }

    if (!texData)
        return 0;

    GLenum format = GL_RGB;
    if (bitDepth == 1) format = GL_RED;
    else if (bitDepth == 3) format = GL_RGB;
    else if (bitDepth == 4) format = GL_RGBA;

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (texture->mHeight == 0)
        stbi_image_free(texData);

    return textureID;
}

GLuint AnimatedModel::CreateWhiteTexture()
{
    if (whiteTextureID != 0)
        return whiteTextureID;

    unsigned char whitePixel[] = { 255, 255, 255, 255 };
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

void AnimatedModel::ReadMissingBones(const aiAnimation* animation)
{
    bones.clear();

    for (unsigned int i = 0; i < animation->mNumChannels; i++)
    {
        const aiNodeAnim* channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            if (boneCounter >= MAX_ANIMATED_BONES)
                continue;
            boneInfoMap[boneName].id = boneCounter;
            boneInfoMap[boneName].offset = glm::mat4(1.0f);
            boneCounter++;
        }
        bones.push_back(AnimatedBone(channel->mNodeName.data, boneInfoMap[boneName].id, channel));
    }
}

void AnimatedModel::ReadHeirarchyData(AnimatedNodeData& dest, const aiNode* src)
{
    dest.name = src->mName.data;
    dest.transformation = ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (unsigned int i = 0; i < src->mNumChildren; i++)
    {
        AnimatedNodeData newData;
        ReadHeirarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}

AnimatedBone* AnimatedModel::FindBone(const std::string& name)
{
    for (auto& bone : bones)
    {
        if (bone.GetBoneName() == name)
            return &bone;
    }
    return nullptr;
}

void AnimatedModel::UpdateAnimation(float deltaTime)
{
    if (!loaded || duration <= 0.0f)
        return;

    currentTime += (float)ticksPerSecond * deltaTime;
    currentTime = fmod(currentTime, duration);
    CalculateBoneTransform(&rootNode, glm::mat4(1.0f));
}

void AnimatedModel::ResetAnimation()
{
    if (!loaded || duration <= 0.0f)
        return;

    currentTime = 0.0f;
    CalculateBoneTransform(&rootNode, glm::mat4(1.0f));
}

void AnimatedModel::SetAnimationProgress(float progress)
{
    if (!loaded || duration <= 0.0f)
        return;

    if (progress < 0.0f)
        progress = 0.0f;
    if (progress > 1.0f)
        progress = 1.0f;

    // Evita que el tiempo caiga exactamente en duration y se reinicie o extrapole.
    // Visualmente representa el ultimo frame de la animacion.
    float tiempoMaximo = duration - 0.0001f;
    if (tiempoMaximo < 0.0f)
        tiempoMaximo = 0.0f;

    currentTime = tiempoMaximo * progress;
    CalculateBoneTransform(&rootNode, glm::mat4(1.0f));
}

void AnimatedModel::CalculateBoneTransform(const AnimatedNodeData* node, const glm::mat4& parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    AnimatedBone* bone = FindBone(nodeName);
    if (bone)
    {
        bone->Update(currentTime);
        nodeTransform = bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoIt = boneInfoMap.find(nodeName);
    if (boneInfoIt != boneInfoMap.end())
    {
        int index = boneInfoIt->second.id;
        if (index >= 0 && index < MAX_ANIMATED_BONES)
            finalBoneMatrices[index] = globalInverseTransform * globalTransformation * boneInfoIt->second.offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}

void AnimatedModel::SetBoneUniforms(GLuint shaderID)
{
    for (int i = 0; i < MAX_ANIMATED_BONES; ++i)
    {
        std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
        GLint location = glGetUniformLocation(shaderID, name.c_str());
        if (location != -1)
            glUniformMatrix4fv(location, 1, GL_FALSE, &finalBoneMatrices[i][0][0]);
    }
}

void AnimatedModel::RenderModel()
{
    if (!loaded)
        return;

    for (unsigned int i = 0; i < meshList.size(); i++)
    {
        unsigned int materialIndex = meshList[i]->GetMaterialIndex();
        GLuint textureID = whiteTextureID;
        if (materialIndex < textureIDList.size() && textureIDList[materialIndex] != 0)
            textureID = textureIDList[materialIndex];

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        meshList[i]->RenderMesh();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void AnimatedModel::ClearModel()
{
    for (unsigned int i = 0; i < meshList.size(); i++)
    {
        if (meshList[i])
        {
            delete meshList[i];
            meshList[i] = nullptr;
        }
    }
    meshList.clear();

    for (GLuint textureID : textureIDList)
    {
        if (textureID != 0 && textureID != whiteTextureID)
            glDeleteTextures(1, &textureID);
    }
    textureIDList.clear();

    if (whiteTextureID != 0)
    {
        glDeleteTextures(1, &whiteTextureID);
        whiteTextureID = 0;
    }
}

AnimatedModel::~AnimatedModel()
{
    ClearModel();
}

glm::mat4 AnimatedModel::ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

glm::vec3 AnimatedModel::GetGLMVec(const aiVector3D& vec)
{
    return glm::vec3(vec.x, vec.y, vec.z);
}

glm::quat AnimatedModel::GetGLMQuat(const aiQuaternion& pOrientation)
{
    return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}
