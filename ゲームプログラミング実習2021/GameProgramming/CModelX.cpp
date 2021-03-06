#include "glut.h"
#include <stdio.h>
#include "CModelX.h"
#include <string.h>
#include"CModel.h"
#include"CMatrix.h"

void CModelX::Load(char *file){
	//
	//ファイルサイズを取得する
	//
	FILE*fp;  //ファイルポインタ変数の作成

	fp = fopen(file, "rb"); //ファイルをオープンする
	if (fp == NULL){       //エラーチェック
		printf("fopen error:%s\n", file);
		return;
	}

	//ファイルの最後へ移動
	fseek(fp, 0L, SEEK_END);
	//ファイルサイズの取得
	int size = ftell(fp);
	//ファイルサイズ+1バイト分の領域を確保
	char*buf = mpPointer = new char[size + 1];
	//
	//ファイルから3Dモデルのデータを読み込む
	//
	//ファイルの先頭へ移動
	fseek(fp, 0L, SEEK_SET);
	//確保した領域にファイルサイズ分データを読み込む
	fread(buf, size, 1, fp);
	//最後に\0を設定する(文字列の終端)
	buf[size] = '\0';
	fclose(fp);    //ファイルをクローズする

	
	//文字列の最後まで繰り返し
	while (*mpPointer != '\0'){
		GetToken();//単語の取得
		//単語がFrameの場合
		if (strcmp(mToken, "Frame") == 0){
			//フレームを作成する
			new CModelXFrame(this);
			
		}

		if (strcmp(mToken, "AnimationSet") == 0){
				printf("%s", mToken);
				GetToken();
				printf("%s\n", mToken);   
		}
	}
		SAFE_DELETE_ARRAY(buf);   //確保した領域を開放する}
	
}



/*
GetToken
文字列データから、単語を1つ取得する
*/
void CModelX::GetToken(){
	char* p = mpPointer;
	char* q = mToken;
	//空白()タブ(\t)改行(\r)(\n).:゛以外の文字を飛ばす
	/*
	strchr(文字列、文字)
	文字列に文字が含まれていれば、見つかった文字へのポインタを返す
	見つからなかったらNULLを返す
	*/
	while (*p != '\0'&& strchr(" \t\r\n,;\"",*p))p++;
	if (*p == '{'||*p == '}'){
		//{または}ならmTokenに代入し次の文字へ
		*q++ = *p++;
	}
	else{
		//空白()タブ(\t)改行(\r)(\n).;"}の文字になるまでmTokenに代入する
		while (*p != '\0'&& !strchr(" \t\r\n,;\"}", *p))
			*q++ = *p++;
	}
	*q = '\0';  //mTokenの最後に\０を代入
	mpPointer = p; //次の読み込むポイントを更新する	　
    //もしmTokenが//の場合は、コメントなので改行まで読み飛ばす
	/*
	strcmp(文字列１、文字列２)
	文字列１と文字列２が等しくない場合、0を返します
	文字列１と文字列２が等しくない場合、0以外を返します。
	*/
	if (!strcmp("//", mToken)){
		//改行まで読み飛ばす
		while (*p != '\0'&& !strchr("\r\n", *p))p++;
		//読み込み位置の更新
		mpPointer = p;
		//単語を取得する(再帰呼び出し)
		GetToken();
	}
}

/*
SkipNode
ノードを読み飛ばす
*/
void CModelX::SkipNode(){
	//文字が終わったら終了
	while (*mpPointer != '\0'){
		GetToken(); //次の単語取得
		//{が見つかったらループ終了
		if (strchr(mToken, '{'))break;
	}

	int count = 1;
	//文字が終わるか、カウントが０になったら終了
	while (mpPointer != '\0'&& count > 0){
		GetToken();
		//{を見つけるとカウントアップ
		if (strchr(mToken, '{'))count++;
		//}を見つけるとカウントダウン
		else if (strchr(mToken, '}'))count--;
	}
}
/*
CModelXFrame
model CModelXインスタンスへのポインタ
フレームを作成する
読み込み中にFrameが見つかれば、フレームを作成し、
子フレームに追加する
*/
CModelXFrame::CModelXFrame(CModelX* model){
	//現在のフレーム配列の要素数を取得し設定する
	mIndex = model->mFrame.size();
	//CModelXのフレーム配列に追加する
	model->mFrame.push_back(this);
	//変換行列を単位行列にする
	mTransformMatrix.Identity();
	//次の単語(フレーム名の予定)を取得する
	model->GetToken();//frame name
	//フレーム名文エリアを確保する
	mpName = new char[strlen(model->mToken) + 1];
	//フレーム名をコピーする
	strcpy(mpName, model->mToken);
	//次の単語({の予定)を取得する
	model->GetToken();//{
	//文字がなくなったら終わり
	while (*model->mpPointer != '\0'){
		//次の単語取得
		model->GetToken();//Frame
		//}かっこの場合は終了
		if (strchr(model->mToken, '}'))break;
		if (strcmp(model->mToken, "Frame") == 0){


			//フレームを作成し、子フレームの配列に追加
			mChild.push_back(
				new CModelXFrame(model));
		}
		else if (strcmp(model->mToken, "FrameTransformMatrix") == 0){

			model->GetToken();//{
			for (int i = 0; i < ARRAY_SIZE(mTransformMatrix.mF); i++){
				mTransformMatrix.mF[i] = model->GetFloatToken();
			}
			model->GetToken();//}
		}
		else if (strcmp(model->mToken, "Mesh") == 0){
			mMesh.Init(model);
		}
		else{
			//上記以外の要素は読み飛ばす
			model->SkipNode();
		}
	}

	//デバッグバージョンのみ有効
#ifndef _ DEBUG
	printf("%s\n", mpName);
	mTransformMatrix.Print();
#endif	
}
/*GetFloatToken
単語を浮動小数点型のデータで返す
*/
		float CModelX::GetFloatToken(){
			GetToken();
			//atof
			//文字列がfloat型へ変換
			return atof(mToken);
		}
		/*
		GetIntToken
		単語を整数型のデータで返す
		*/
		int CModelX::GetIntToken(){
			GetToken();
			return atoi(mToken);
		}
		/*
		Init
		Meshのデータを取り込む
		*/
		void CMesh::Init(CModelX*model){
			model->GetToken();  //{ or 名前
			if (!strchr(model->mToken, '{')){
				//名前の場合、次が{
				model->GetToken();  //{
			}
			//頂点数の取得
			mVertexNum = model->GetIntToken();
			//頂点数分エリア確保
			mpVertex = new CVector[mVertexNum];
			//頂点数分データを取り込む
			for (int i = 0; i < mVertexNum; i++){
				mpVertex[i].mX = model->GetFloatToken();
				mpVertex[i].mY = model->GetFloatToken();
				mpVertex[i].mZ = model->GetFloatToken();
			}
			printf("VertexNum:%d\n", mVertexNum);
			for (int i = 0; i <mVertexNum; i++)

			{
				printf("%10f", mpVertex[i].mX);
				printf("%10f", mpVertex[i].mY);
				printf("%10f\n", mpVertex[i].mZ);
			}
			mFaceNum = model->GetIntToken();
			//頂点数は１面に３頂点
			mpVertexIndex = new int[mFaceNum * 3];
			for (int i = 0; i < mFaceNum * 3; i += 3){
				model->GetToken();//頂点数読み飛ばし
				mpVertexIndex[i] = model->GetIntToken();
				mpVertexIndex[i+1] = model->GetIntToken();
				mpVertexIndex[i+2] = model->GetIntToken();
			}
			printf("FaceNum:%d\n", mFaceNum);
			for (int i = 0; i < mFaceNum * 3;i+=3)

			{
				printf("%4d", mpVertexIndex[i]);
				printf("%4d", mpVertexIndex[i+1]);
				printf("%4d\n", mpVertexIndex[i+2]);
			}
			model->GetToken();//MeshNormals
			if (strcmp(model->mToken, "MeshNormals") == 0){
				model->GetToken();//{
				//法線データ数を取得
				mNormalNum = model->GetIntToken();
				//法線データを配列に取り込む
				CVector*pNormal = new CVector[mNormalNum];
				for (int i = 0; i < mNormalNum; i++){
					pNormal[i].mX = model->GetFloatToken();
					pNormal[i].mY = model->GetFloatToken();
					pNormal[i].mZ = model->GetFloatToken();
				}
				printf("NormalNum:%d\n", mNormalNum);
				for (int i = 0; i < mNormalNum; i++)

				{
					printf("%10f", pNormal[i].mX);
					printf("%10f", pNormal[i].mY);
					printf("%10f\n", pNormal[i].mZ);
				}
				//法線数＝画数*3
				mNormalNum = model->GetIntToken() * 3;//FaceNum
				int ni;
				mpNormal = new CVector[mNormalNum];
				for (int i = 0; i < mNormalNum; i += 3){
					model->GetToken();//3
					ni = model->GetIntToken();
					mpNormal[i] = pNormal[ni];

					ni = model->GetIntToken();
					mpNormal[i+1] = pNormal[ni];

					ni = model->GetIntToken();
					mpNormal[i+2] = pNormal[ni];
				}

				delete[]pNormal;
				model->GetToken();//}
	}
}