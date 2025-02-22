#include "TriMesh.h"
#include "Angel.h"
#include "Camera.h"
#include <vector>
#include "MeshPainter.h"
#include <string>
#include <algorithm>
#include <assert.h>
#include <stb_image.h>
#include <map>
#define White	glm::vec3(1.0, 1.0, 1.0)
#define Yellow	glm::vec3(1.0, 1.0, 0.0)
#define Green	glm::vec3(0.0, 1.0, 0.0)
#define Cyan	glm::vec3(0.0, 1.0, 1.0)
#define Magenta	glm::vec3(1.0, 0.0, 1.0)
#define Red		glm::vec3(1.0, 0.0, 0.0)
#define Black	glm::vec3(0.0, 0.0, 0.0)
#define Blue	glm::vec3(0.0, 0.0, 1.0)
#define Brown	glm::vec3(0.5, 0.5, 0.5)

namespace main_var {
	struct openGLObject
	{
		// 顶点数组对象
		GLuint vao;
		// 顶点缓存对象
		GLuint vbo;

		// 着色器程序
		GLuint program;
		// 着色器文件
		std::string vshader;
		std::string fshader;
		// 着色器变量
		GLuint pLocation;
		GLuint cLocation;
		GLuint nLocation;
		GLuint tLocation;

		// 纹理
		std::string texture_image;
		GLuint texture;

		// 投影变换变量
		GLuint modelLocation;
		GLuint viewLocation;
		GLuint projectionLocation;

		// 阴影变量
		GLuint shadowLocation;
		// 是否使用纹理绘制变量
		GLuint isTextureLocation;
	};
	void bindObjectAndData(TriMesh* mesh, openGLObject& object, const std::string&, const std::string& vshader, const std::string& fshader);
	void bindLightAndMaterial(TriMesh* mesh, openGLObject& object, Light* light, Camera* camera);
	void drawMesh(glm::mat4 modelMatrix, TriMesh* mesh, openGLObject object, int);
	void load_texture_STBImage(const std::string& file_name, GLuint& texture);;
}


int WIDTH = 1600;
int HEIGHT = 1000;

int mainWindow;

// 矩阵的栈，无需改动
class MatrixStack {
	int		_index;
    int		_size;
    glm::mat4*	_matrices;

public:
	MatrixStack(int numMatrices = 100):_index(0), _size(numMatrices)
        { _matrices = new glm::mat4[numMatrices]; }

    ~MatrixStack()
		{ delete[]_matrices; }

    void push(const glm::mat4& m){
		assert( _index + 1 < _size );
		_matrices[_index++] = m;	 
    }

	glm::mat4& pop(){
        assert(_index - 1 >= 0);
        _index--;
        return _matrices[_index];
    }
};

struct Robot
{
	// 关节大小
	float TORSO_HEIGHT = 4.0;
	float TORSO_WIDTH = 2.5;
	float UPPER_ARM_HEIGHT = 2.5;
	float LOWER_ARM_HEIGHT = 1.8;
	float UPPER_ARM_WIDTH =  0.8;
	float LOWER_ARM_WIDTH =  0.5;
	float UPPER_LEG_HEIGHT = 2.8;
	float LOWER_LEG_HEIGHT = 2.2;
	float UPPER_LEG_WIDTH =  1.0;
	float LOWER_LEG_WIDTH =  0.5;
	float HEAD_HEIGHT = 1.8;
	float HEAD_WIDTH = 1.5;

	// 关节角和菜单选项值
	enum {
		Torso = 1,		// 躯干
		Head,			// 头部
		LeftUpperArm,	// 左大臂
		LeftLowerArm,	// 左小臂
		Sword,			// 剑
		RightUpperArm,	// 右大臂
		RightLowerArm,	// 右小臂
		LeftUpperLeg,	// 左大腿
		LeftLowerLeg,	// 左小腿	
		RightUpperLeg,	// 右大腿
		RightLowerLeg,	// 右小腿
	};

	// 关节角大小
	GLfloat theta[10] = {
		0.0,    // Torso
		0.0,    // Head
		0.0,    // RightUpperArm
		0.0,    // RightLowerArm
		0.0,    // LeftUpperArm
		0.0,    // LeftLowerArm
		0.0,    // RightUpperLeg
		0.0,    // RightLowerLeg
		0.0,    // LeftUpperLeg
		0.0     // LeftLowerLeg
	};
};
Robot robot;

// 相机
Camera* camera = new Camera();
// 光照
Light* light = new Light();
// 地板
TriMesh* plane = new TriMesh();
// 房子
TriMesh* house1 = new TriMesh();
TriMesh* house2 = new TriMesh();
TriMesh* house3 = new TriMesh();
TriMesh* house4 = new TriMesh();
// 教堂
TriMesh* church = new TriMesh();
// 水井
TriMesh* well = new TriMesh();
// 村民1
TriMesh* villger1 = new TriMesh();
TriMesh* villger2 = new TriMesh();
TriMesh* villger3 = new TriMesh();
// 集市
TriMesh* market1 = new TriMesh();
TriMesh* market2 = new TriMesh();
// 白云
TriMesh* cloud1 = new TriMesh();
TriMesh* cloud2 = new TriMesh();
// 农舍
TriMesh* farmhouse = new TriMesh();
MeshPainter* painter = new MeshPainter();
// 获取生成的所有模型，用于结束程序时释放内存
std::vector<TriMesh*> meshList;

// 机器人
TriMesh* Torso = new TriMesh();
TriMesh* Head = new TriMesh();
TriMesh* RightUpperArm = new TriMesh();
TriMesh* RightLowerArm = new TriMesh();
TriMesh* LeftUpperArm = new TriMesh();
TriMesh* LeftLowerArm = new TriMesh();
TriMesh* RightUpperLeg = new TriMesh();
TriMesh* RightLowerLeg = new TriMesh();
TriMesh* LeftUpperLeg = new TriMesh();
TriMesh* LeftLowerLeg = new TriMesh();
TriMesh* Sword = new TriMesh();
main_var::openGLObject TorsoObject;
main_var::openGLObject HeadObject;
main_var::openGLObject RightUpperArmObject;
main_var::openGLObject RightLowerArmObject;
main_var::openGLObject LeftUpperArmObject;
main_var::openGLObject LeftLowerArmObject;
main_var::openGLObject RightUpperLegObject;
main_var::openGLObject RightLowerLegObject;
main_var::openGLObject LeftUpperLegObject;
main_var::openGLObject LeftLowerLegObject;

// 物体的索引
std::map<std::string, int> meshIndexMap;
// 下标对应上述模型的索引
glm::vec3 meshScale[101] = { glm::vec3(0, 0, 0) };
GLfloat Theta[101] = { 0 };

// 可控制的模型的数量
const int canBeSelectedNum = 100;
// 可控制的模型
std::map<int, std::string> selectedMeshMap;
// 当前控制的mesh的索引
int selectedMesh = 0;
// 当前控制的mesh的名字
std::string selectedMeshName = "camera";

void main_var::drawMesh(glm::mat4 modelMatrix, TriMesh* mesh, openGLObject object, int isTextureorNot=1) {
	
	glBindVertexArray(object.vao);
	
	glUseProgram(object.program);
	
 //   // 父节点矩阵 * 本节点局部变换矩阵
    glUniformMatrix4fv( object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv( object.viewLocation, 1, GL_FALSE, &camera->viewMatrix[0][0]);
	glUniformMatrix4fv( object.projectionLocation, 1, GL_FALSE, &camera->projMatrix[0][0]);
	glUniform1i( object.shadowLocation, 0);
	// 决定使用材质还是纹理
	glUniform1i(object.isTextureLocation, isTextureorNot);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object.texture);// 该语句必须，否则将只使用同一个纹理进行绘制
	// 传递纹理数据 将生成的纹理传给shader
	glUniform1i(glGetUniformLocation(object.program, "texture"), 0);
	// 将材质和光源数据传递给着色器
	bindLightAndMaterial(mesh, object, light, camera);
	// 绘制
	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());

	// @MYDO
	modelMatrix = light->getShadowProjectionMatrix() * modelMatrix;
	// 传递矩阵
	glUniformMatrix4fv(object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	// 将着色器 isShadow 设置为0，表示正常绘制的颜色，如果是1着表示阴影
	glUniform1i(object.shadowLocation, 1);
	// 绘制
	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());
}

// 躯体
void torso(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, 0.5 * robot.TORSO_HEIGHT, 0.0));
	instance = glm::scale(instance, glm::vec3(robot.TORSO_WIDTH, robot.TORSO_HEIGHT, robot.TORSO_WIDTH));
	
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, Torso, TorsoObject);
}

// 头部
void head(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, 0.5 * robot.HEAD_HEIGHT, 0.0));
	instance = glm::scale(instance, glm::vec3(robot.HEAD_WIDTH, robot.HEAD_HEIGHT, robot.HEAD_WIDTH));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, Head, HeadObject, 0);
}


// 左大臂
void left_upper_arm(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.UPPER_ARM_HEIGHT, 0.0));
	instance = glm::scale(instance, glm::vec3(robot.UPPER_ARM_WIDTH, robot.UPPER_ARM_HEIGHT, robot.UPPER_ARM_WIDTH));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, LeftUpperArm, LeftUpperArmObject);
}


// @TODO: 左小臂
void left_lower_arm(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.LOWER_ARM_HEIGHT, 0.0));
	instance = glm::scale(instance, glm::vec3(robot.LOWER_ARM_WIDTH, robot.LOWER_ARM_HEIGHT, robot.LOWER_ARM_WIDTH));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, LeftLowerArm, LeftLowerArmObject);
}

// @TODO: 右大臂
void right_upper_arm(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.UPPER_ARM_HEIGHT, 0.0));
	instance = glm::scale(instance, glm::vec3(robot.UPPER_ARM_WIDTH, robot.UPPER_ARM_HEIGHT, robot.UPPER_ARM_WIDTH));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, RightUpperArm, RightUpperArmObject);
}

// @TODO: 右小臂
void right_lower_arm(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.LOWER_ARM_HEIGHT, 0.0));
	instance = glm::scale(instance, glm::vec3(robot.LOWER_ARM_WIDTH, robot.LOWER_ARM_HEIGHT, robot.LOWER_ARM_WIDTH));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, RightLowerArm, RightLowerArmObject);
}

// @TODO: 左大腿
void left_upper_leg(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.UPPER_LEG_HEIGHT, 0.0));
	instance = glm::scale(instance, glm::vec3(robot.UPPER_LEG_WIDTH, robot.UPPER_LEG_HEIGHT, robot.UPPER_LEG_WIDTH));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, LeftUpperLeg, LeftUpperLegObject);
}

// @TODO: 左小腿
void left_lower_leg(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.LOWER_LEG_HEIGHT, 0.0));
	instance = glm::scale(instance, glm::vec3(robot.LOWER_LEG_WIDTH, robot.LOWER_LEG_HEIGHT, robot.LOWER_LEG_WIDTH));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, LeftLowerLeg, LeftLowerLegObject);
}

// @TODO: 右大腿
void right_upper_leg(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.UPPER_LEG_HEIGHT, 0.0));
	instance = glm::scale(instance, glm::vec3(robot.UPPER_LEG_WIDTH, robot.UPPER_LEG_HEIGHT, robot.UPPER_LEG_WIDTH));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, RightUpperLeg, RightUpperLegObject);
}

// @TODO: 右小腿
void right_lower_leg(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.LOWER_LEG_HEIGHT, 0.0));
	instance = glm::scale(instance, glm::vec3(robot.LOWER_LEG_WIDTH, robot.LOWER_LEG_HEIGHT, robot.LOWER_LEG_WIDTH));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, RightLowerLeg, RightLowerLegObject);
}

void main_var::load_texture_STBImage(const std::string& file_name, GLuint& texture) {
	// 读取纹理图片，并将其传递给着色器

	int width, height, channels = 0;
	unsigned char* pixels = NULL;
	// 读取图片的时候先翻转一下图片，如果不设置的话显示出来是反过来的图片
	stbi_set_flip_vertically_on_load(true);
	// 读取图片数据
	pixels = stbi_load(file_name.c_str(), &width, &height, &channels, 0);

	// 调整行对齐格式
	if (width * channels % 4 != 0)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLenum format = GL_RGB;
	// 设置通道格式
	switch (channels)
	{
	case 1:
		format = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		format = GL_RGB;
		break;
	}

	// 绑定纹理对象
	glBindTexture(GL_TEXTURE_2D, texture);

	// 指定纹理的放大，缩小滤波，使用线性方式，即当图片放大的时候插值方式
	// 将图片的rgb数据上传给opengl
	glTexImage2D(
		GL_TEXTURE_2D,    // 指定目标纹理，这个值必须是GL_TEXTURE_2D
		0,                // 执行细节级别，0是最基本的图像级别，n表示第N级贴图细化级别
		format,           // 纹理数据的颜色格式(GPU显存)
		width,            // 宽度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
		height,           // 高度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
		0,                // 指定边框的宽度。必须为0
		format,           // 像素数据的颜色格式(CPU内存)
		GL_UNSIGNED_BYTE, // 指定像素数据的数据类型
		pixels            // 指定内存中指向图像数据的指针
	);

	// 生成多级渐远纹理，多消耗1/3的显存，较小分辨率时获得更好的效果
	// glGenerateMipmap(GL_TEXTURE_2D);

	// 指定插值方法
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// 恢复初始对齐格式
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	// 释放图形内存
	stbi_image_free(pixels);
};

void main_var::bindObjectAndData(TriMesh* mesh, openGLObject& object, const std::string& texture_image, const std::string& vshader, const std::string& fshader) {
	// 初始化各种对象

	std::vector<glm::vec3> points = mesh->getPoints();
	std::vector<glm::vec3> normals = mesh->getNormals();
	std::vector<glm::vec3> colors = mesh->getColors();
	std::vector<glm::vec2> textures = mesh->getTextures();

	// 创建顶点数组对象
#ifdef __APPLE__	// for MacOS
	glGenVertexArraysAPPLE(1, &object.vao);		// 分配1个顶点数组对象
	glBindVertexArrayAPPLE(object.vao);		// 绑定顶点数组对象
#else				// for Windows
	glGenVertexArrays(1, &object.vao);  	// 分配1个顶点数组对象
	glBindVertexArray(object.vao);  	// 绑定顶点数组对象
#endif

	// 创建并初始化顶点缓存对象
	glGenBuffers(1, &object.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glBufferData(GL_ARRAY_BUFFER,
		points.size() * sizeof(glm::vec3) +
		normals.size() * sizeof(glm::vec3) +
		colors.size() * sizeof(glm::vec3) +
		textures.size() * sizeof(glm::vec2),
		NULL, GL_STATIC_DRAW);

	// 绑定顶点数据
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec3), points.data());
	// 绑定颜色数据
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), colors.size() * sizeof(glm::vec3), colors.data());
	// 绑定法向量数据
	glBufferSubData(GL_ARRAY_BUFFER, (points.size() + colors.size()) * sizeof(glm::vec3), normals.size() * sizeof(glm::vec3), normals.data());
	// 绑定纹理数据
	glBufferSubData(GL_ARRAY_BUFFER, (points.size() + normals.size() + colors.size()) * sizeof(glm::vec3), textures.size() * sizeof(glm::vec2), textures.data());


	object.vshader = vshader;
	object.fshader = fshader;
	object.program = InitShader(object.vshader.c_str(), object.fshader.c_str());

	// 将顶点传入着色器
	object.pLocation = glGetAttribLocation(object.program, "vPosition");
	glEnableVertexAttribArray(object.pLocation);
	glVertexAttribPointer(object.pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// 将颜色传入着色器
	object.cLocation = glGetAttribLocation(object.program, "vColor");
	glEnableVertexAttribArray(object.cLocation);
	glVertexAttribPointer(object.cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(glm::vec3)));

	// 将法向量传入着色器
	object.nLocation = glGetAttribLocation(object.program, "vNormal");
	glEnableVertexAttribArray(object.nLocation);
	glVertexAttribPointer(object.nLocation, 3,
		GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET((points.size() + colors.size()) * sizeof(glm::vec3)));

	object.tLocation = glGetAttribLocation(object.program, "vTexture");
	glEnableVertexAttribArray(object.tLocation);
	glVertexAttribPointer(object.tLocation, 2,
		GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET((points.size() + colors.size() + normals.size()) * sizeof(glm::vec3)));


	// 获得矩阵位置
	object.modelLocation = glGetUniformLocation(object.program, "model");
	object.viewLocation = glGetUniformLocation(object.program, "view");
	object.projectionLocation = glGetUniformLocation(object.program, "projection");

	object.shadowLocation = glGetUniformLocation(object.program, "isShadow");
	object.isTextureLocation = glGetUniformLocation(object.program, "isTexture");


	// 读取纹理图片数
	object.texture_image = texture_image;
	// 创建纹理的缓存对象
	glGenTextures(1, &object.texture);
	// 调用stb_image生成纹理
	load_texture_STBImage(object.texture_image, object.texture);
	// Clean up
	glUseProgram(0);
#ifdef __APPLE__
	glBindVertexArrayAPPLE(0);
#else
	glBindVertexArray(0);
#endif
}

void main_var::bindLightAndMaterial(TriMesh* mesh, openGLObject& object, Light* light, Camera* camera) {

	// 传递相机的位置
	glUniform3fv(glGetUniformLocation(object.program, "eye_position"), 1, &camera->eye[0]);

	// 传递物体的材质
	glm::vec4 meshAmbient = mesh->getAmbient();
	glm::vec4 meshDiffuse = mesh->getDiffuse();
	glm::vec4 meshSpecular = mesh->getSpecular();
	float meshShininess = mesh->getShininess();

	glUniform4fv(glGetUniformLocation(object.program, "material.ambient"), 1, &meshAmbient[0]);
	glUniform4fv(glGetUniformLocation(object.program, "material.diffuse"), 1, &meshDiffuse[0]);
	glUniform4fv(glGetUniformLocation(object.program, "material.specular"), 1, &meshSpecular[0]);
	glUniform1f(glGetUniformLocation(object.program, "material.shininess"), meshShininess);

	// 传递光源信息
	glm::vec4 lightAmbient = light->getAmbient();
	glm::vec4 lightDiffuse = light->getDiffuse();
	glm::vec4 lightSpecular = light->getSpecular();
	glm::vec3 lightPosition = light->getTranslation();
	glUniform4fv(glGetUniformLocation(object.program, "light.ambient"), 1, &lightAmbient[0]);
	glUniform4fv(glGetUniformLocation(object.program, "light.diffuse"), 1, &lightDiffuse[0]);
	glUniform4fv(glGetUniformLocation(object.program, "light.specular"), 1, &lightSpecular[0]);
	glUniform3fv(glGetUniformLocation(object.program, "light.position"), 1, &lightPosition[0]);

}

void setADSS(TriMesh* mesh, glm::vec4 mat_ambient, glm::vec4 mat_diffuse, glm::vec4 mat_specular, float shine)
{
	mesh->setAmbient(mat_ambient); // 环境光
	mesh->setDiffuse(mat_diffuse); // 漫反射
	mesh->setSpecular(mat_specular); // 镜面反射
	mesh->setShininess(shine); //高光系数
}
void init()
{
	std::string vshader, tfshader, noPhongfshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	tfshader = "shaders/tfshader.glsl";
	noPhongfshader = "shaders/NoPhongfshader.glsl";

	int index = 0, selectedMeshMapIndex = 0;
	meshIndexMap["camera"] = 100;
	// 相机 控制索引0
	selectedMeshMap[selectedMeshMapIndex++] = "camera";

	// 0 设置光源
	meshIndexMap["light"] = index++;
	light->readObj("assets/sun/sun.obj");
	light->setTranslation(glm::vec3(20.0, 30.0f, 20.0));
	light->setScale(glm::vec3(2, 2, 2));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(glm::vec4(0.5, 0.5, 0.5, 0.5)); // 镜面反射
	painter->addMesh(light, "light", "assets/sun/sun.png", vshader, tfshader);
	meshList.push_back(light);

	// 1 地板
	meshIndexMap["plane"] = index++;
	plane->setNormalize(false);
	plane->readObj("assets/plane/plane.obj");
	plane->setTranslation(glm::vec3(0.0, -0.1, 0.0));    //加偏移，防止跟阴影重合
	plane->setRotation(glm::vec3(180, 0.0, 0.0));
	meshScale[meshIndexMap["plane"]] = glm::vec3(25.0, 25.0, 25.0);
	plane->setScale(meshScale[meshIndexMap["plane"]]);
	plane->setAmbient(glm::vec4(0.5, 0.5, 0.5, 1.0)); // 环境光
	plane->setDiffuse(glm::vec4(0.5, 0.5, 0.5, 1.0)); // 漫反射
	plane->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	plane->setShininess(1.0); //高光系数
	painter->addMesh(plane, "plane", "assets/plane/plane.png", vshader, tfshader);
	meshList.push_back(plane);

	// 机器人的各个部位
	glm::vec3 robotScale = glm::vec3(0.02, 0.02, 0.02);
	// 2 躯干 控制索引1
	meshIndexMap["Torso"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "Torso";
	Torso->setNormalize(false);
	Torso->readObj("assets/robot/body.obj");
	Torso->setTranslation(glm::vec3(-8.0f, 0.0f, -0.f));
	meshScale[meshIndexMap["Torso"]] = robotScale;
	Torso->setScale(meshScale[meshIndexMap["Torso"]]);
	painter->addMesh(Torso, "Torso", "assets/robot/body.jpg", vshader, tfshader);
	meshList.push_back(Torso);
	// 3 头 控制索引2
	meshIndexMap["Head"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "Head";
	Head->setNormalize(false);
	//Head->readObj("assets/robot/Head.obj");
	Head->generateCube();
	meshScale[meshIndexMap["Head"]] = glm::vec3(4.5, 4.5, 4.5);
	Head->setScale(meshScale[meshIndexMap["Head"]]);
	painter->addMesh(Head, "Head", "assets/robot/head.png", vshader, tfshader);
	meshList.push_back(Head);
	// 4 左上臂 控制索引3
	meshIndexMap["LeftUpperArm"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "LeftUpperArm";
	LeftUpperArm->setNormalize(false);
	LeftUpperArm->readObj("assets/robot/LeftUpperHand.obj");
	meshScale[meshIndexMap["LeftUpperArm"]] = robotScale;
	LeftUpperArm->setScale(meshScale[meshIndexMap["LeftUpperArm"]]);
	Theta[meshIndexMap["LeftUpperArm"]] = 5;
	painter->addMesh(LeftUpperArm, "LeftUpperArm", "assets/robot/another.jpg", vshader, tfshader);
	meshList.push_back(LeftUpperArm);
	// 5 左下臂 控制索引4
	meshIndexMap["LeftLowerArm"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "LeftLowerArm";
	LeftLowerArm->setNormalize(false);
	LeftLowerArm->readObj("assets/robot/LeftLowerHand.obj");
	meshScale[meshIndexMap["LeftLowerArm"]] = robotScale;
	LeftLowerArm->setScale(meshScale[meshIndexMap["LeftLowerArm"]]);
	Theta[meshIndexMap["LeftLowerArm"]] = -15;
	painter->addMesh(LeftLowerArm, "LeftLowerArm", "assets/robot/another.jpg", vshader, tfshader);
	meshList.push_back(LeftLowerArm);
	// 6 剑 控制索引5
	meshIndexMap["Sword"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "Sword";
	Sword->setNormalize(false);
	Sword->readObj("assets/sword/test.obj");
	meshScale[meshIndexMap["Sword"]] = robotScale;
	Sword->setScale(meshScale[meshIndexMap["Sword"]]);
	painter->addMesh(Sword, "Sword", "assets/sword/test.jpg", vshader, tfshader);
	meshList.push_back(Sword);
	// 7 右上臂 控制索引6
	meshIndexMap["RightUpperArm"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "RightUpperArm";
	RightUpperArm->setNormalize(false);
	RightUpperArm->readObj("assets/robot/RightUpperHand.obj");
	meshScale[meshIndexMap["RightUpperArm"]] = robotScale;
	RightUpperArm->setScale(meshScale[meshIndexMap["RightUpperArm"]]);
	Theta[meshIndexMap["RightUpperArm"]] = 5;
	painter->addMesh(RightUpperArm, "RightUpperArm", "assets/robot/another.jpg", vshader, tfshader);
	meshList.push_back(RightUpperArm);
	// 8 右下臂 控制索引7
	meshIndexMap["RightLowerArm"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "RightLowerArm";
	RightLowerArm->setNormalize(false);
	RightLowerArm->readObj("assets/robot/RightLowerHand.obj");
	meshScale[meshIndexMap["RightLowerArm"]] = robotScale;
	RightLowerArm->setScale(meshScale[meshIndexMap["RightLowerArm"]]);
	Theta[meshIndexMap["RightLowerArm"]] = -15;
	painter->addMesh(RightLowerArm, "RightLowerArm", "assets/robot/another.jpg", vshader, tfshader);
	meshList.push_back(RightLowerArm);
	// 9 左大腿 控制索引8
	meshIndexMap["LeftUpperLeg"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "LeftUpperLeg";
	LeftUpperLeg->setNormalize(false);
	LeftUpperLeg->readObj("assets/robot/LeftUpperLeg.obj");
	meshScale[meshIndexMap["LeftUpperLeg"]] = robotScale;
	LeftUpperLeg->setScale(meshScale[meshIndexMap["LeftUpperLeg"]]);
	painter->addMesh(LeftUpperLeg, "LeftUpperLeg", "assets/robot/another.jpg", vshader, tfshader);
	meshList.push_back(LeftUpperLeg);
	// 10 左小腿 控制索引9
	meshIndexMap["LeftLowerLeg"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "LeftLowerLeg";
	LeftLowerLeg->setNormalize(false);
	LeftLowerLeg->readObj("assets/robot/LeftLowerLeg.obj");
	meshScale[meshIndexMap["LeftLowerLeg"]] = robotScale;
	LeftLowerLeg->setScale(meshScale[meshIndexMap["LeftLowerLeg"]]);
	painter->addMesh(LeftLowerLeg, "LeftLowerLeg", "assets/robot/another.jpg", vshader, tfshader);
	meshList.push_back(LeftLowerLeg);
	// 11 右大腿 控制索引10
	meshIndexMap["RightUpperLeg"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "RightUpperLeg";
	RightUpperLeg->setNormalize(false);
	RightUpperLeg->readObj("assets/robot/RightUpperLeg.obj");
	meshScale[meshIndexMap["RightUpperLeg"]] = robotScale;
	RightUpperLeg->setScale(meshScale[meshIndexMap["RightUpperLeg"]]);
	painter->addMesh(RightUpperLeg, "RightUpperLeg", "assets/robot/another.jpg", vshader, tfshader);
	meshList.push_back(RightUpperLeg);
	// 12 右小腿  控制索引11
	meshIndexMap["RightLowerLeg"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "RightLowerLeg";
	RightLowerLeg->setNormalize(false);
	RightLowerLeg->readObj("assets/robot/RightLowerLeg.obj");
	meshScale[meshIndexMap["RightLowerLeg"]] = robotScale;
	RightLowerLeg->setScale(meshScale[meshIndexMap["RightLowerLeg"]]);
	painter->addMesh(RightLowerLeg, "RightLowerLeg", "assets/robot/another.jpg", vshader, tfshader);
	meshList.push_back(RightLowerLeg);

	// 13 房子
	meshIndexMap["house1"] = index++;
	house1->setNormalize(false);
	house1->readObj("assets/houses/house1.obj");
	meshScale[meshIndexMap["house1"]] = glm::vec3(2, 2, 2);
	house1->setScale(meshScale[meshIndexMap["house1"]]);
	painter->addMesh(house1, "house1", "assets/houses/house1.png", vshader, tfshader);
	meshList.push_back(house1);

	// 14 教堂
	meshIndexMap["church"] = index++;
	church->setNormalize(false);
	church->readObj("assets/church/church.obj");
	meshScale[meshIndexMap["church"]] = glm::vec3(2, 2, 2);
	church->setScale(meshScale[meshIndexMap["church"]]);
	painter->addMesh(church, "church", "assets/church/church.png", vshader, tfshader);
	meshList.push_back(church);

	// 15 水井
	meshIndexMap["well"] = index++;
	well->setNormalize(false);
	well->readObj("assets/well/well.obj");
	meshScale[meshIndexMap["well"]] = glm::vec3(1, 1, 1);
	well->setScale(meshScale[meshIndexMap["well"]]);
	painter->addMesh(well, "well", "assets/well/well.jpg", vshader, tfshader);
	meshList.push_back(well);

	// 16 村民1 控制索引12
	meshIndexMap["villger1"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "villger1";
	villger1->setNormalize(false);
	villger1->readObj("assets/villgers/villger1.obj");
	meshScale[meshIndexMap["villger1"]] = glm::vec3(0.9, 0.9, 0.9);
	villger1->setScale(meshScale[meshIndexMap["villger1"]]);
	painter->addMesh(villger1, "villger1", "assets/villgers/villger1.jpg", vshader, tfshader);
	meshList.push_back(villger1);

	// 17 房子2
	meshIndexMap["house2"] = index++;
	house2->setNormalize(false);
	house2->readObj("assets/houses/house2.obj");
	meshScale[meshIndexMap["house2"]] = glm::vec3(1.5, 1.5, 1.5);
	house2->setScale(meshScale[meshIndexMap["house2"]]);
	painter->addMesh(house2, "house2", "assets/houses/house2.png", vshader, tfshader);
	meshList.push_back(house2);

	// 18 房子3
	meshIndexMap["house3"] = index++;
	house3->setNormalize(false);
	house3->readObj("assets/houses/house3.obj");
	meshScale[meshIndexMap["house3"]] = glm::vec3(1.5, 1.5, 1.5);
	house3->setScale(meshScale[meshIndexMap["house3"]]);
	painter->addMesh(house3, "house3", "assets/houses/house3.png", vshader, tfshader);
	meshList.push_back(house3);

	// 19 房子4
	meshIndexMap["house4"] = index++;
	house4->setNormalize(false);
	house4->readObj("assets/houses/house4.obj");
	meshScale[meshIndexMap["house4"]] = glm::vec3(1.5, 1.5, 1.5);
	house4->setScale(meshScale[meshIndexMap["house4"]]);
	painter->addMesh(house4, "house4", "assets/houses/house4.png", vshader, tfshader);
	meshList.push_back(house4);

	// 20 集市1
	meshIndexMap["market1"] = index++;
	market1->setNormalize(false);
	market1->readObj("assets/markets/market1.obj");
	meshScale[meshIndexMap["market1"]] = glm::vec3(1.5, 1.5, 1.5);
	market1->setScale(meshScale[meshIndexMap["market1"]]);
	painter->addMesh(market1, "market1", "assets/markets/market1.png", vshader, tfshader);
	meshList.push_back(market1);

	// 21 集市2
	meshIndexMap["market2"] = index++;
	market2->setNormalize(false);
	market2->readObj("assets/markets/market2.obj");
	meshScale[meshIndexMap["market2"]] = glm::vec3(2.7, 2.7, 2.7);
	market2->setScale(meshScale[meshIndexMap["market2"]]);
	painter->addMesh(market2, "market2", "assets/markets/market2.png", vshader, tfshader);
	meshList.push_back(market2);

	// 22 村民2 控制索引13
	meshIndexMap["villger2"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "villger2";
	villger2->setNormalize(false);
	villger2->readObj("assets/villgers/villger2.obj");
	meshScale[meshIndexMap["villger2"]] = glm::vec3(0.9, 0.9, 0.9);
	Theta[meshIndexMap["villger2"]] = -135;
	villger2->setScale(meshScale[meshIndexMap["villger2"]]);
	painter->addMesh(villger2, "villger2", "assets/villgers/villger2.jpg", vshader, tfshader);
	meshList.push_back(villger2);

	// 23 村民3 控制索引14
	meshIndexMap["villger3"] = index++;
	selectedMeshMap[selectedMeshMapIndex++] = "villger3";
	villger3->setNormalize(false);
	villger3->readObj("assets/villgers/villger3.obj");
	meshScale[meshIndexMap["villger3"]] = glm::vec3(0.9, 0.9, 0.9);
	Theta[meshIndexMap["villger3"]] = 135;
	villger3->setScale(meshScale[meshIndexMap["villger3"]]);
	painter->addMesh(villger3, "villger3", "assets/villgers/villger3.jpg", vshader, tfshader);
	meshList.push_back(villger3);

	// 24 白云1
	meshIndexMap["cloud1"] = index++;
	cloud1->setNormalize(false);
	cloud1->readObj("assets/clouds/cloud1.obj");
	meshScale[meshIndexMap["cloud1"]] = glm::vec3(2, 2, 2);
	//cloud2->setTranslation(glm::vec3(15.0, 30.0f, 20.0));
	cloud1->setScale(meshScale[meshIndexMap["cloud1"]]);
	painter->addMesh(cloud1, "cloud1", "assets/clouds/cloud1.png", vshader, tfshader);
	meshList.push_back(cloud1);

	// 25 白云2
	meshIndexMap["cloud2"] = index++;
	cloud2->setNormalize(false);
	cloud2->readObj("assets/clouds/cloud2.obj");
	meshScale[meshIndexMap["cloud2"]] = glm::vec3(1.5, 1.5, 1.5);
	//cloud2->setTranslation(glm::vec3(5.0, 25.0f, 15.0));
	cloud2->setScale(meshScale[meshIndexMap["cloud2"]]);
	painter->addMesh(cloud2, "cloud2", "assets/clouds/cloud2.png", vshader, tfshader);
	meshList.push_back(cloud2);

	// 26 农舍
	meshIndexMap["farmhouse"] = index++;
	farmhouse->setNormalize(false);
	farmhouse->readObj("assets/farmhouse/farmhouse.obj");
	meshScale[meshIndexMap["farmhouse"]] = glm::vec3(2, 2, 2);
	farmhouse->setScale(meshScale[meshIndexMap["farmhouse"]]);
	painter->addMesh(farmhouse, "farmhouse", "assets/farmhouse/farmhouse.png", vshader, tfshader);
	meshList.push_back(farmhouse);

	glClearColor(0.34, 0.64, 0.98, 1.0);
}
float shengtian = 0.0;
void drawRobot()
{
	float bias = 0.5;

	// 躯干
	MatrixStack mstack;
	glm::mat4 modelMatrix = Torso->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, shengtian + LeftLowerLeg->getHeight() + LeftUpperLeg->getHeight() - bias, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Theta[meshIndexMap["Torso"]]), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(meshIndexMap["Torso"], modelMatrix, light, camera, 1);

	// 头
	mstack.push(modelMatrix);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, LeftLowerLeg->getHeight() + LeftUpperLeg->getHeight() + 1.5 * Head->getHeight(), 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Theta[meshIndexMap["Head"]]), glm::vec3(0.0, 1.0, 0.0));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["Head"]]);
	painter->drawMesh(meshIndexMap["Head"], modelMatrix, light, camera, 1);

	// 左大臂
	modelMatrix = mstack.pop();
	mstack.push(modelMatrix);
	modelMatrix = glm::translate(modelMatrix, glm::vec3((Torso->getLength() + LeftUpperArm->getLength()) / 2, Torso->getHeight() - LeftUpperArm->getHeight(), 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, LeftUpperArm->getHeight(), 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Theta[meshIndexMap["LeftUpperArm"]]), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -LeftUpperArm->getHeight(), 0.0));
	//modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["LeftUpperArm"]]);
	painter->drawMesh(meshIndexMap["LeftUpperArm"], modelMatrix, light, camera, 1);

	// 左小臂
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -LeftLowerArm->getHeight() + bias, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, LeftLowerArm->getHeight(), 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Theta[meshIndexMap["LeftLowerArm"]]), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -LeftLowerArm->getHeight(), 0.0));
	painter->drawMesh(meshIndexMap["LeftLowerArm"], modelMatrix, light, camera, 1);

	// 剑
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0. * (LeftLowerArm->getHeight() + LeftLowerArm->getHeight()), 5.0));
	/*modelMatrix = glm::translate(modelMatrix, glm::vec3(-LeftLowerArm->getLength() * 7.5, Sword->getHeight() * 12, 0.0));*/
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0. * (LeftLowerArm->getHeight() + LeftLowerArm->getHeight()), -5.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(45 + Theta[meshIndexMap["Sword"]]), glm::vec3(1.0, 0.0, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
	/*modelMatrix = glm::translate(modelMatrix, glm::vec3(LeftLowerArm->getLength() * 7.5, -Sword->getHeight() * 12, 0.0));*/
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0. * (LeftLowerArm->getHeight() + LeftLowerArm->getHeight()), 5.0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(15, 15, 15));
	painter->drawMesh(meshIndexMap["Sword"], modelMatrix, light, camera, 1);

	// 右大臂
	modelMatrix = mstack.pop();
	mstack.push(modelMatrix);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-(Torso->getLength() + RightUpperArm->getLength()) * 41 / 80, Torso->getHeight() - RightUpperArm->getHeight(), 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, RightUpperArm->getHeight(), 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Theta[meshIndexMap["RightUpperArm"]]), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -RightUpperArm->getHeight(), 0.0));
	//modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["RightUpperArm"]]);
	painter->drawMesh(meshIndexMap["RightUpperArm"], modelMatrix, light, camera, 1);

	// 右小臂
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -RightLowerArm->getHeight() + bias, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, LeftLowerArm->getHeight(), 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Theta[meshIndexMap["RightLowerArm"]]), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -LeftLowerArm->getHeight(), 0.0));
	painter->drawMesh(meshIndexMap["RightLowerArm"], modelMatrix, light, camera, 1);

	// 左大腿
	modelMatrix = mstack.pop();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, bias / 2, 0.0));
	mstack.push(modelMatrix);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(Torso->getLength() * 13.0f / 45.0f, -LeftUpperLeg->getHeight(), 0.0));
	// 为了得到正确的旋转，需要先将腿往下移，绕原点旋转，再恢复原位
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, LeftUpperLeg->getHeight(), 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Theta[meshIndexMap["LeftUpperLeg"]]), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -LeftUpperLeg->getHeight(), 0.0));
	//modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["LeftUpperLeg"]]);
	painter->drawMesh(meshIndexMap["LeftUpperLeg"], modelMatrix, light, camera, 1);

	// 左小腿
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -LeftLowerLeg->getHeight() + bias / 2, 0.0));
	// 为了得到正确的旋转，需要先将腿往下移，绕原点旋转，再恢复原位
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, LeftLowerLeg->getHeight(), 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Theta[meshIndexMap["LeftLowerLeg"]]), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -LeftLowerLeg->getHeight(), 0.0));
	//modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["LeftLowerLeg"]]);
	painter->drawMesh(meshIndexMap["LeftLowerLeg"], modelMatrix, light, camera, 1);

	// 右大腿
	modelMatrix = mstack.pop();
	mstack.push(modelMatrix);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-Torso->getLength() * 13.0f / 45.0f, -RightUpperLeg->getHeight(), 0.0));
	// 为了得到正确的旋转，需要先将腿往下移，绕原点旋转，再恢复原位
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, RightUpperLeg->getHeight(), 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Theta[meshIndexMap["RightUpperLeg"]]), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -RightUpperLeg->getHeight(), 0.0));
	//modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["RightUpperLeg"]]);
	painter->drawMesh(meshIndexMap["RightUpperLeg"], modelMatrix, light, camera, 1);

	// 右小腿
	//modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["RightLowerLeg"]]);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -RightLowerLeg->getHeight() + bias / 2, 0.0));
	// 为了得到正确的旋转，需要先将腿往下移，绕原点旋转，再恢复原位
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, RightLowerLeg->getHeight(), 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Theta[meshIndexMap["RightLowerLeg"]]), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -RightLowerLeg->getHeight(), 0.0));
	painter->drawMesh(meshIndexMap["RightLowerLeg"], modelMatrix, light, camera, 1);

	//// 物体的变换矩阵
	//glm::mat4 modelMatrix = glm::mat4(1.0);

	//// 保持变换矩阵的栈
	//MatrixStack mstack;

	//// 躯干（这里我们希望机器人的躯干只绕Y轴旋转，所以只计算了RotateY）
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.5 * (robot.LOWER_LEG_HEIGHT + robot.UPPER_LEG_HEIGHT), 0.0));
	//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5, 0.5, 0.5));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.Torso]), glm::vec3(0.0, 1.0, 0.0));
	//torso(modelMatrix);

	//mstack.push(modelMatrix); // 保存躯干变换矩阵
	//// 头部（这里我们希望机器人的头部只绕Y轴旋转，所以只计算了RotateY）
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, robot.TORSO_HEIGHT, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.Head]), glm::vec3(0.0, 1.0, 0.0));
	//head(modelMatrix);
	//modelMatrix = mstack.pop(); // 恢复躯干变换矩阵


	//// =========== 左臂 ===========
	//mstack.push(modelMatrix);   // 保存躯干变换矩阵
	//// 左大臂（这里我们希望机器人的左大臂只绕Z轴旋转，所以只计算了RotateZ，后面同理）
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5 * robot.TORSO_WIDTH - 0.5 * robot.UPPER_ARM_WIDTH, robot.TORSO_HEIGHT, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.LeftUpperArm]), glm::vec3(1.0, 0.0, 0.0));
	//left_upper_arm(modelMatrix);

	//// @TODO: 左小臂
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -robot.UPPER_ARM_HEIGHT, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.LeftLowerArm]), glm::vec3(1.0, 0.0, 0.0));
	//left_lower_arm(modelMatrix);

	//modelMatrix = mstack.pop();
	//// =========== 右臂 ===========
	//mstack.push(modelMatrix);
	//// @TODO: 右大臂
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5 * robot.TORSO_WIDTH + 0.5 * robot.UPPER_ARM_WIDTH, robot.TORSO_HEIGHT, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.RightUpperArm]), glm::vec3(1.0, .0, 0.0));
	//right_upper_arm(modelMatrix);

	//// @TODO: 右小臂
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -robot.UPPER_ARM_HEIGHT, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.RightLowerArm]), glm::vec3(1.0, 0.0, 0.0));
	//right_lower_arm(modelMatrix);

	//modelMatrix = mstack.pop();
	//// =========== 左腿 ===========
	//mstack.push(modelMatrix);
	//// @TODO: 左大腿
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35 * robot.TORSO_WIDTH, 0.0, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.LeftUpperLeg]), glm::vec3(1.0, 0.0, 0.0));
	//left_upper_leg(modelMatrix);

	//// @TODO: 左小腿
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -robot.UPPER_LEG_HEIGHT, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.LeftLowerLeg]), glm::vec3(1.0, 0.0, 0.0));
	//left_lower_leg(modelMatrix);

	//modelMatrix = mstack.pop();
	//// =========== 右腿 ===========
	//mstack.push(modelMatrix);
	//// @TODO: 右大腿
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35 * robot.TORSO_WIDTH, 0.0, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.RightUpperLeg]), glm::vec3(1.0, 0.0, 0.0));
	//right_upper_leg(modelMatrix);

	//// @TODO: 右小腿
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -robot.UPPER_LEG_HEIGHT, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.RightLowerLeg]), glm::vec3(1.0, 0.0, 0.0));
	//right_lower_leg(modelMatrix);

	//modelMatrix = mstack.pop();
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 物体的变换矩阵
	glm::mat4 modelMatrix = glm::mat4(1.0);

	// 光源
	modelMatrix = light->getModelMatrix();
	painter->drawMesh(meshIndexMap["light"], modelMatrix, light, camera, 0);

	// 地板
	modelMatrix = plane->getModelMatrix();
	painter->drawMesh(meshIndexMap["plane"], modelMatrix, light, camera, 0);

	// 机器人
	drawRobot();
	
	// 房子
	modelMatrix = house1->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.01, 0));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["house1"]]);
	painter->drawMesh(meshIndexMap["house1"], modelMatrix, light, camera, 1);

	modelMatrix = house2->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-2, -0.1, 7));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["house2"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(GLfloat(-90.0)), glm::vec3(0.0, -1.0, 0.0));
	painter->drawMesh(meshIndexMap["house2"], modelMatrix, light, camera, 1);

	modelMatrix = house3->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(1, -0.1, 7));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["house3"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(GLfloat(180.0)), glm::vec3(0.0, -1.0, 0.0));
	painter->drawMesh(meshIndexMap["house3"], modelMatrix, light, camera, 1);

	modelMatrix = house4->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-3, -0.1, -5));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["house4"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(GLfloat(90.0)), glm::vec3(0.0, -1.0, 0.0));
	painter->drawMesh(meshIndexMap["house4"], modelMatrix, light, camera, 1);

	// 教堂
	modelMatrix = church->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(5, -0.01, 3));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["church"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(GLfloat(90.0)), glm::vec3(0.0, -1.0, 0.0));
	painter->drawMesh(meshIndexMap["church"], modelMatrix, light, camera, 1);

	// 水井
	modelMatrix = church->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-6, -0.015, 2));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["well"]]);
	painter->drawMesh(meshIndexMap["well"], modelMatrix, light, camera, 1);

	// 村民
	modelMatrix = villger1->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-1, -0.01, 4));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["villger1"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90+Theta[meshIndexMap["villger1"]]), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(meshIndexMap["villger1"], modelMatrix, light, camera, 1);

	modelMatrix = villger2->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(6, -0.01, 8));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["villger2"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90 + Theta[meshIndexMap["villger2"]]), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(meshIndexMap["villger2"], modelMatrix, light, camera, 1);

	modelMatrix = villger3->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-6, -0.01, 8));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["villger3"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90 + Theta[meshIndexMap["villger3"]]), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(meshIndexMap["villger3"], modelMatrix, light, camera, 1);

	// 集市
	modelMatrix = market1->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(6, -0.01, 0));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["market1"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(GLfloat(90.0)), glm::vec3(0.0, -1.0, 0.0));
	painter->drawMesh(meshIndexMap["market1"], modelMatrix, light, camera, 1);

	modelMatrix = market2->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-6, -0.05, 4));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["market2"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(GLfloat(30.0)), glm::vec3(0.0, -1.0, 0.0));
	painter->drawMesh(meshIndexMap["market2"], modelMatrix, light, camera, 1);

	modelMatrix = market2->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-6.5, -0.05, -2));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["market2"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(GLfloat(90.0)), glm::vec3(0.0, -1.0, 0.0));
	painter->drawMesh(meshIndexMap["market2"], modelMatrix, light, camera, 1);

	// 白云
	modelMatrix = cloud1->getModelMatrix();
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["cloud1"]]);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-15.0, 6.0f, 5.0));
	painter->drawMesh(meshIndexMap["cloud1"], modelMatrix, light, camera, 0);

	modelMatrix = cloud2->getModelMatrix();
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["cloud2"]]);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-10.0, 6.0f, -5.0));
	painter->drawMesh(meshIndexMap["cloud2"], modelMatrix, light, camera, 0);

	// 农舍
	modelMatrix = farmhouse->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(1, -0.1, -8));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["farmhouse"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(GLfloat(90.0)), glm::vec3(0.0, -1.0, 0.0));
	painter->drawMesh(meshIndexMap["farmhouse"], modelMatrix, light, camera, 1);

	modelMatrix = farmhouse->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(4, -0.1, -8));
	modelMatrix = glm::scale(modelMatrix, meshScale[meshIndexMap["farmhouse"]]);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(GLfloat(90.0)), glm::vec3(0.0, -1.0, 0.0));
	painter->drawMesh(meshIndexMap["farmhouse"], modelMatrix, light, camera, 1);
}


void printHelp()
{

	std::cout << "================================================" << std::endl << std::endl;
	std::cout << "Use right click to open Menu." << std::endl;
	std::cout << "================================================" << std::endl << std::endl;

	std::cout << "Keyboard Usage" << std::endl;
	std::cout <<
		"[Window]" << std::endl <<
		"ESC:		Exit" << std::endl <<
		"h:		Print help message" << std::endl <<
		std::endl <<

		"[Part]" << std::endl <<
		"1:		Torso" << std::endl <<
		"2:		Head" << std::endl <<
		"3:		RightUpperArm" << std::endl <<
		"4:		RightLowerArm" << std::endl <<
		"5:     Sword" << std::endl <<
		"6:		LeftUpperArm" << std::endl <<
		"7:		LeftLowerArm" << std::endl <<
		"8:		RightUpperLeg" << std::endl <<
		"9:		RightLowerLeg" << std::endl <<
		"0:		LeftUpperLeg" << std::endl <<
		"-:		LeftLowerLeg" << std::endl <<
		std::endl <<

		"[Model]" << std::endl <<
		"CTRL:	    Increase rotate angle" << std::endl <<
		"ALT:	    Decrease rotate angle" << std::endl <<
		"SPACE:		select mesh" << std::endl <<

		std::endl <<
		"[Camera]" << std::endl <<
		"CTRL:		rise" << std::endl <<
		"ALT:       descend" << std::endl <<
		"W:		    advance" << std::endl <<
		"S:		    recoil" << std::endl <<
		"A:			turn left" << std::endl <<
		"D:			turn right" << std::endl <<
		std::endl <<

		"[light]" << std::endl <<
		"I/J/K/L:	control direction" << std::endl << std::endl <<
		std::endl;



}


// 键盘响应函数
std::map<std::string, bool> KeyMap;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	float tmp;
	glm::vec4 ambient;
	if (action == GLFW_PRESS || action == GLFW_REPEAT || action == GLFW_RELEASE) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE: 
			KeyMap["ESCAPE"] = true; 
			break;
		case GLFW_KEY_1: 
			selectedMesh = robot.Torso; 
			selectedMeshName = selectedMeshMap[selectedMesh];
			break;
		case GLFW_KEY_2: 
			selectedMesh = robot.Head;
			selectedMeshName = selectedMeshMap[selectedMesh];
			break;
		case GLFW_KEY_3: 
			selectedMesh = robot.LeftUpperArm;
			selectedMeshName = selectedMeshMap[selectedMesh];
			break;
		case GLFW_KEY_4:
			selectedMesh = robot.LeftLowerArm;  
			selectedMeshName = selectedMeshMap[selectedMesh];
			break;
		case GLFW_KEY_5: 
			selectedMesh = robot.Sword;
			selectedMeshName = selectedMeshMap[selectedMesh];
			break;
		case GLFW_KEY_6: 
			selectedMesh = robot.RightUpperArm;
			selectedMeshName = selectedMeshMap[selectedMesh];
			break;
		case GLFW_KEY_7: 
			selectedMesh = robot.RightLowerArm; 
			selectedMeshName = selectedMeshMap[selectedMesh];
			break;
		case GLFW_KEY_8: 
			selectedMesh = robot.LeftUpperLeg; 
			selectedMeshName = selectedMeshMap[selectedMesh];
			break;
		case GLFW_KEY_9: 	
			selectedMesh = robot.LeftLowerLeg; 
			selectedMeshName = selectedMeshMap[selectedMesh];
			break;
		case GLFW_KEY_0: 
			selectedMesh = robot.RightUpperLeg; 
			selectedMeshName = selectedMeshMap[selectedMesh];
			break;
		case GLFW_KEY_MINUS: 
			selectedMesh = robot.RightLowerLeg; 
			selectedMeshName = selectedMeshMap[selectedMesh];
			break;
		// 通过按键旋转
		case GLFW_KEY_LEFT_CONTROL:
			if (action == GLFW_PRESS) {
				KeyMap["CTRL"] = true;
			}
			else if (action == GLFW_RELEASE) {
				KeyMap["CTRL"] = false;
			}
			break;

		case GLFW_KEY_LEFT_ALT:
			if (action == GLFW_PRESS) {
				KeyMap["ALT"] = true;
			}
			else if (action == GLFW_RELEASE) {
				KeyMap["ALT"] = false;
			}
			break;
		// 光源控制
		case GLFW_KEY_I:
			if (action == GLFW_PRESS) 
				KeyMap["I"] = true;
			else if (action == GLFW_RELEASE) 
				KeyMap["I"] = false;
			break;
		case GLFW_KEY_K:
			if (action == GLFW_PRESS) 
				KeyMap["K"] = true;
			else if (action == GLFW_RELEASE) 
				KeyMap["K"] = false;
			break;
		case GLFW_KEY_J:
			if (action == GLFW_PRESS) 
				KeyMap["J"] = true;
			else if (action == GLFW_RELEASE) 
				KeyMap["J"] = false;
			break;
		case GLFW_KEY_L:
			if (action == GLFW_PRESS) 
				KeyMap["L"] = true;
			else if (action == GLFW_RELEASE) 
				KeyMap["L"] = false;
			break;
		// 相机
		case GLFW_KEY_SPACE:
			if (action == GLFW_PRESS) {
				if (selectedMesh == 2) {
					//camera->radius = camera->radius + 10;
					selectedMesh = 0;
					selectedMeshName = selectedMeshMap[selectedMesh];
				}
				else if (selectedMesh == 0) {
					selectedMesh = 12;
					selectedMeshName = selectedMeshMap[selectedMesh];
				}
				else if (selectedMesh == 12) {
					selectedMesh = 13;
					selectedMeshName = selectedMeshMap[selectedMesh];
				}
				else if (selectedMesh == 13) {
					selectedMesh = 14;
					selectedMeshName = selectedMeshMap[selectedMesh];
				}
				else if (selectedMesh == 14) {
					selectedMesh = 1;
					selectedMeshName = selectedMeshMap[selectedMesh];
				}
				else {
					selectedMesh = 0;
					selectedMeshName = selectedMeshMap[selectedMesh];
				}
			}
			break;
		// WASD控制移动方向
		case GLFW_KEY_W:
		case GLFW_KEY_S:
		case GLFW_KEY_A:
		case GLFW_KEY_D:
			if (action == GLFW_PRESS)
			{
				if (key == GLFW_KEY_W) 
					KeyMap["W"] = true;
				else if (key == GLFW_KEY_S) 
					KeyMap["S"] = true;
				else if (key == GLFW_KEY_A) 
					KeyMap["A"] = true;
				else if (key == GLFW_KEY_D) 
					KeyMap["D"] = true;
			}
			else if (action == GLFW_RELEASE)
			{
				if (key == GLFW_KEY_W) 
					KeyMap["W"] = false;
				else if (key == GLFW_KEY_S) 
					KeyMap["S"] = false;
				else if (key == GLFW_KEY_A) 
					KeyMap["A"] = false;
				else if (key == GLFW_KEY_D) 
					KeyMap["D"] = false;
			}
			break;
		}
	}
}
int direction = -1;
void processKeyInput(GLFWwindow* window)
{
	// 窗口控制
	if (KeyMap["ESCAPE"])
		exit(EXIT_SUCCESS);
	// 光源控制
	if (KeyMap["I"] || KeyMap["J"] || KeyMap["K"] || KeyMap["L"])
	{
		glm::vec3 translation = light->getTranslation();
		// 获得移动方向
		int zDirection = 0, xDirection = 0;
		if (KeyMap["I"]) {
			zDirection = -1;
		}
		else if (KeyMap["K"]) {
			zDirection = 1;
		}
		else if (KeyMap["J"]) {
			xDirection = -1;
		}
		else if (KeyMap["L"]) {
			xDirection = 1;
		}

		// 空气墙
		float dist = 0.1;
		if (translation.z + zDirection * dist < 25.f && translation.z + zDirection * dist > -25.f) translation.z += zDirection * dist;
		if (translation.x + xDirection * dist < 25.f && translation.x + xDirection * dist > -25.f) translation.x += xDirection * dist;
		light->setTranslation(glm::vec3(translation.x, translation.y, translation.z));
	}
	if (selectedMeshMap[selectedMesh] != "camera") {
		if (meshIndexMap[selectedMeshName] >= 3 && meshIndexMap[selectedMeshName] <= 12) {
			if (KeyMap["CTRL"]) {
				//shengtian += 0.01 * LeftLowerLeg->getHeight();
				Theta[meshIndexMap[selectedMeshName]] += 2.0;
				if (Theta[meshIndexMap[selectedMeshName]] > 90) {
					Theta[meshIndexMap[selectedMeshName]] = 90;
				}
				if (selectedMeshName == "LeftLowerArm" && Theta[meshIndexMap[selectedMeshName]] >= 5) {
					Theta[meshIndexMap[selectedMeshName]] = 5;
				}
				else if (selectedMeshName == "RightLowerArm" && Theta[meshIndexMap[selectedMeshName]] >= 5) {
					Theta[meshIndexMap[selectedMeshName]] = 5;
				}
				else if (selectedMeshName == "Sword" && Theta[meshIndexMap[selectedMeshName]] >= 30) {
					Theta[meshIndexMap[selectedMeshName]] = 30;
				}
			}
			else if (KeyMap["ALT"]) {
				Theta[meshIndexMap[selectedMeshName]] -= 2.0;
				if (Theta[meshIndexMap[selectedMeshName]] < -90.0) {
					Theta[meshIndexMap[selectedMeshName]] = -90;
				}
				if (selectedMeshName == "LeftLowerLeg" && Theta[meshIndexMap[selectedMeshName]] <= -5) {
					Theta[meshIndexMap[selectedMeshName]] = -5;
				}
				else if (selectedMeshName == "RightLowerLeg" && Theta[meshIndexMap[selectedMeshName]] <= -5) {
					Theta[meshIndexMap[selectedMeshName]] = -5;
				}
				else if (selectedMeshName == "Sword" && Theta[meshIndexMap[selectedMeshName]] <= -30) {
					Theta[meshIndexMap[selectedMeshName]] = -30;
				}
			}
		}
		if (!(meshIndexMap[selectedMeshName] >= 3 && meshIndexMap[selectedMeshName] <= 12) && (KeyMap["W"] || KeyMap["S"] || KeyMap["A"] || KeyMap["D"]))
		{
			float dist = 0.05;
			glm::vec3 translation;
			// 获取mesh的traslation矩阵
			if (selectedMeshName == "Torso") translation = Torso->getTranslation();
			else if (selectedMeshName == "villger1") translation = villger1->getTranslation();
			else if (selectedMeshName == "villger2") translation = villger2->getTranslation();
			else if (selectedMeshName == "villger3") translation = villger3->getTranslation();
			// 摄像机需要旋转的角度（与人物一致）
			GLfloat angle = Theta[meshIndexMap[selectedMeshName]];
			// W/S控制前进后退，A/D控制左右转向
			if (KeyMap["W"])
				translation += glm::vec3(dist * sin(glm::radians(angle)), 0.0, dist * cos(glm::radians(angle)));
			if (KeyMap["S"])
				translation -= glm::vec3(dist * sin(glm::radians(angle)), 0.0, dist * cos(glm::radians(angle)));
			if (KeyMap["A"])
			{
				Theta[meshIndexMap[selectedMeshName]] += 2.0;
				if (Theta[meshIndexMap[selectedMeshName]] > 360.0f) Theta[meshIndexMap[selectedMeshName]] -= 360.0f;
			}
			if (KeyMap["D"])
			{
				Theta[meshIndexMap[selectedMeshName]] -= 2.0;
				if (Theta[meshIndexMap[selectedMeshName]] < 0.0f) Theta[meshIndexMap[selectedMeshName]] += 360.0f;
			}
			// 记得保存T矩阵
			// 空气墙
			translation.x = std::min(25.f, translation.x);
			translation.x = std::max(-25.f, translation.x);
			translation.z = std::min(25.f, translation.z);
			translation.z = std::max(-25.f, translation.z);
			if (selectedMeshName == "Torso") {
				if (Theta[meshIndexMap["LeftUpperArm"]] > 45.0 || Theta[meshIndexMap["LeftUpperArm"]] < -45.0)
				{
					//控制摆动方向
					direction = -direction;
				}
				float moveSpeed = 1.5;
				Theta[meshIndexMap["LeftUpperArm"]] += direction * moveSpeed;
				Theta[meshIndexMap["RightUpperArm"]] -= direction * moveSpeed;
				Theta[meshIndexMap["LeftUpperLeg"]] -= direction * moveSpeed;
				Theta[meshIndexMap["RightUpperLeg"]] += direction * moveSpeed;

				Torso->setTranslation(translation);
			}
			else if (selectedMeshName == "villger1") {
				villger1->setTranslation(translation);
			}	
			else if (selectedMeshName == "villger2") {
				villger2->setTranslation(translation);
			}
			else if (selectedMeshName == "villger3") {
				villger3->setTranslation(translation);
			}
		}
	}
	else {
		camera->keyboard(window);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//鼠标控制摄像机
	camera->mouse(xpos, ypos);
}

void cleanData() {
	
	// 释放内存
	delete camera;
	camera = NULL;

	for (int i=0; i<meshList.size(); i++) {
		meshList[i]->cleanData();
		delete meshList[i];
	}
	meshList.clear();

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(int argc, char **argv)
{
	// 初始化GLFW库，必须是应用程序调用的第一个GLFW函数
	glfwInit();

	// 配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// 配置窗口属性
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "2022155028_黄亮铭_期末作业", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// 鼠标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	// 调用任何OpenGL的函数之前初始化GLAD
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Init mesh, shaders, buffer
	init();

	// 输出帮助信息
	printHelp();
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		processKeyInput(window);
		display();
		// 交换颜色缓冲 以及 检查有没有触发什么事件（比如键盘输入、鼠标移动等）
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanData();


	return 0;
}

// 每当窗口改变大小，GLFW会调用这个函数并填充相应的参数供你处理。
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
