#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

const int MAX_BONE_INFLUENCE = 4;
const int MAX_ANIMATED_BONES = 100;

struct AnimatedVertex
{
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
    int BoneIDs[MAX_BONE_INFLUENCE];
    float Weights[MAX_BONE_INFLUENCE];

    AnimatedVertex()
    {
        Position = glm::vec3(0.0f);
        TexCoords = glm::vec2(0.0f);
        Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            BoneIDs[i] = -1;
            Weights[i] = 0.0f;
        }
    }
};

struct BoneInfo
{
    int id;
    glm::mat4 offset;
};

struct AnimatedNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AnimatedNodeData> children;
};

struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

class AnimatedBone
{
public:
    AnimatedBone() = default;
    AnimatedBone(const std::string& name, int id, const aiNodeAnim* channel);

    void Update(float animationTime);
    glm::mat4 GetLocalTransform() const { return localTransform; }
    std::string GetBoneName() const { return name; }
    int GetBoneID() const { return id; }

private:
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    glm::mat4 InterpolatePosition(float animationTime);
    glm::mat4 InterpolateRotation(float animationTime);
    glm::mat4 InterpolateScaling(float animationTime);
    int GetPositionIndex(float animationTime);
    int GetRotationIndex(float animationTime);
    int GetScaleIndex(float animationTime);

    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;
    int numPositions = 0;
    int numRotations = 0;
    int numScalings = 0;

    glm::mat4 localTransform = glm::mat4(1.0f);
    std::string name;
    int id = -1;
};

class AnimatedMesh
{
public:
    AnimatedMesh();
    void CreateMesh(const std::vector<AnimatedVertex>& vertices, const std::vector<unsigned int>& indices, unsigned int materialIndex);
    void RenderMesh();
    void ClearMesh();
    unsigned int GetMaterialIndex() const { return materialIndex; }
    ~AnimatedMesh();

private:
    GLuint VAO = 0, VBO = 0, IBO = 0;
    GLsizei indexCount = 0;
    unsigned int materialIndex = 0;
};

class AnimatedModel
{
public:
    AnimatedModel();
    void LoadModel(const std::string& fileName);
    void UpdateAnimation(float deltaTime);
    void ResetAnimation();
    void SetAnimationProgress(float progress);
    void RenderModel();
    void SetBoneUniforms(GLuint shaderID);
    void ClearModel();
    bool IsLoaded() const { return loaded; }

    ~AnimatedModel();

private:
    void LoadNode(aiNode* node, const aiScene* scene);
    void LoadMesh(aiMesh* mesh, const aiScene* scene);
    void LoadMaterials(const aiScene* scene, const std::string& modelDirectory);
    void ExtractBoneWeightForVertices(std::vector<AnimatedVertex>& vertices, aiMesh* mesh, const aiScene* scene);
    void SetVertexBoneData(AnimatedVertex& vertex, int boneID, float weight);

    void ReadHeirarchyData(AnimatedNodeData& dest, const aiNode* src);
    void ReadMissingBones(const aiAnimation* animation);
    AnimatedBone* FindBone(const std::string& name);
    void CalculateBoneTransform(const AnimatedNodeData* node, const glm::mat4& parentTransform);

    GLuint LoadTextureFromMaterial(aiMaterial* material, const aiScene* scene, const std::string& modelDirectory);
    GLuint LoadTextureFromFilePath(const std::string& path);
    GLuint LoadEmbeddedTexture(const aiTexture* texture);
    GLuint CreateWhiteTexture();

    glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
    glm::vec3 GetGLMVec(const aiVector3D& vec);
    glm::quat GetGLMQuat(const aiQuaternion& pOrientation);

    std::vector<AnimatedMesh*> meshList;
    std::vector<GLuint> textureIDList;
    std::vector<glm::mat4> finalBoneMatrices;
    std::map<std::string, BoneInfo> boneInfoMap;
    std::vector<AnimatedBone> bones;

    AnimatedNodeData rootNode;
    glm::mat4 globalInverseTransform = glm::mat4(1.0f);

    float duration = 0.0f;
    int ticksPerSecond = 25;
    float currentTime = 0.0f;
    int boneCounter = 0;
    bool loaded = false;
    GLuint whiteTextureID = 0;
};
