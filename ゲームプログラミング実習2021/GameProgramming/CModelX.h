#ifndef CMODELX_H   //インクルードガード
#define CMODELX_H

#define MODEL_FILE "sample.blend.x"//入力ファイル名
//領域解放をマクロ化
#define SAFE_DELETE_ARRAY(a){if(a)delete[]a;a=0;}
//配列のサイズ取得をマクロ化
#define ARRAY_SIZE(a)(sizeof(a)/sizeof(a[0]))
#include <vector>
#include "CMatrix.h"
#include"CVector.h"
class CModelX;
//CMeshクラスの定義
class CMesh{
public:

	int mVertexNum;   //頂点数
	CVector *mpVertex; //頂点データ
	int mFaceNum;//画数
	int *mpVertexIndex;//画を構成する頂点番号
	int mNormalNum;  //法線数
	CVector*mpNormal;//法線データ

	//コンストラクタ
	CMesh()
		:mVertexNum(0)
		, mpVertex(0)
		, mFaceNum(0)
		, mpVertexIndex(nullptr)
		, mNormalNum(0)
		, mpNormal(nullptr)

	{}
	//デストラクタ
	~CMesh(){
		SAFE_DELETE_ARRAY(mpVertex);
		SAFE_DELETE_ARRAY(mpVertexIndex);
		SAFE_DELETE_ARRAY(mpNormal);
	}
	//読み込み処理
	void Init(CModelX*model);



};


//CModelXFrameクラスの定義
class CModelXFrame{
public:
	std::vector<CModelXFrame*>mChild;//子フレームの配列
	CMatrix mTransformMatrix;  //変換行列
	char * mpName;
	int mIndex;
	CMesh mMesh;//Meshデータ
	//コンストラクタ
	CModelXFrame(CModelX*model);
	//デストラクタ
	~CModelXFrame(){
		//子フレームを全て解放する
		std::vector<CModelXFrame*>::iterator itr;
		for (itr = mChild.begin(); itr != mChild.end(); itr++){
			delete*itr;
		}
		//名前のエリアを解放する
		SAFE_DELETE_ARRAY(mpName);
	}
	
};

/*
CModelX
Xファイル形式の3Dモデルデータをプログラムで認識する
*/

//浮動小数点データの取
class CModelX{
public:

	char*mpPointer; //読み込み位置

	char mToken[1024]; //取り出した単語の領域


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
	//単語の取り出し
	void GetToken();
	//浮動小数点データの取得
	float GetFloatToken();
	//ファイル読み込み
	void Load(char*file);
	//ノードの読み飛ばし
	void SkipNode();
	//整数データの取得
	int GetIntToken();

	



};

#endif
