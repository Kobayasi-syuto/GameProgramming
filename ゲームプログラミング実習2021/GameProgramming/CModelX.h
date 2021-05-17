#ifndef CMODELX_H   //�C���N���[�h�K�[�h
#define CMODELX_H

#define MODEL_FILE "sample.blend.x"//���̓t�@�C����
//�̈������}�N����
#define SAFE_DELETE_ARRAY(a){if(a)delete[]a;a=0;}
//�z��̃T�C�Y�擾���}�N����
#define ARRAY_SIZE(a)(sizeof(a)/sizeof(a[0]))
#include <vector>
#include "CMatrix.h"
#include"CVector.h"
class CModelX;
//CMesh�N���X�̒�`
class CMesh{
public:

	int mVertexNum;   //���_��
	CVector *mpVertex; //���_�f�[�^
	int mFaceNum;//�搔
	int *mpVertexIndex;//����\�����钸�_�ԍ�
	int mNormalNum;  //�@����
	CVector*mpNormal;//�@���f�[�^

	//�R���X�g���N�^
	CMesh()
		:mVertexNum(0)
		, mpVertex(0)
		, mFaceNum(0)
		, mpVertexIndex(nullptr)
		, mNormalNum(0)
		, mpNormal(nullptr)

	{}
	//�f�X�g���N�^
	~CMesh(){
		SAFE_DELETE_ARRAY(mpVertex);
		SAFE_DELETE_ARRAY(mpVertexIndex);
		SAFE_DELETE_ARRAY(mpNormal);
	}
	//�ǂݍ��ݏ���
	void Init(CModelX*model);



};


//CModelXFrame�N���X�̒�`
class CModelXFrame{
public:
	std::vector<CModelXFrame*>mChild;//�q�t���[���̔z��
	CMatrix mTransformMatrix;  //�ϊ��s��
	char * mpName;
	int mIndex;
	CMesh mMesh;//Mesh�f�[�^
	//�R���X�g���N�^
	CModelXFrame(CModelX*model);
	//�f�X�g���N�^
	~CModelXFrame(){
		//�q�t���[����S�ĉ������
		std::vector<CModelXFrame*>::iterator itr;
		for (itr = mChild.begin(); itr != mChild.end(); itr++){
			delete*itr;
		}
		//���O�̃G���A���������
		SAFE_DELETE_ARRAY(mpName);
	}
	
};

/*
CModelX
X�t�@�C���`����3D���f���f�[�^���v���O�����ŔF������
*/

//���������_�f�[�^�̎�
class CModelX{
public:

	char*mpPointer; //�ǂݍ��݈ʒu

	char mToken[1024]; //���o�����P��̗̈�


	std::vector<CModelXFrame*>mFrame;

	CModelX()
		:mpPointer(0)
	{}
	~CModelX(){
		if (mFrame.size() > 0)
		{
			delete mFrame[0];
		}
	}
	//�P��̎��o��
	void GetToken();
	//���������_�f�[�^�̎擾
	float GetFloatToken();
	//�t�@�C���ǂݍ���
	void Load(char*file);
	//�m�[�h�̓ǂݔ�΂�
	void SkipNode();
	//�����f�[�^�̎擾
	int GetIntToken();

	



};

#endif
