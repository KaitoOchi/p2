////////////////////////////////////////////////
// ライト用定数バッファの構造体。
////////////////////////////////////////////////
static const int MAX_POINT_LIGHT = 256;
static const int MAX_SPOT_LIGHT = 256;

//一度に実行されるスレッド数。
#define TILE_WIDTH 16
#define TILE_HEIGHT 16

//タイルの総数。
#define TILE_SIZE (TILE_WIDTH * TILE_HEIGHT)

/// <summary>
/// ディレクションライト用の構造体。
/// </summary>
 struct DirectionLig
 {
	float3 dirDirection;	//ライトの方向。
	float3 dirColor;		//ライトのカラー。
	float ambient;			//環境光。
 };

/// <summary>
/// ポイントライト用の構造体。
/// </summary>
 struct PointLig
 {
	float3 ptPosition;		//ライトの位置。
	float3 ptColor;			//ライトのカラー。
	float ptRange;			//ライトの影響範囲。
	float3 posInView;		//カメラ空間での座標。
 };

/// <summary>
/// スポットライト用の構造体。
/// </summary>
 struct SpotLig
 {
	float3 spPosition;		//ライトの位置。
	float3 spColor;			//ライトのカラー。
	float spRange;			//ライトの影響範囲。
	float3 spDirection;		//ライトの方向。
	float spAngle;			//ライトの射出角度。
	float3 posInView;		//カメラ空間での座標。
	float3 directionInView;	//カメラ空間での方向。
 };

/// <summary>
/// シャドウ用の構造体。
/// </summary>
struct ShadowCB
{
	float3 lightPos;		//ライトの座標。
	float4x4 mLVP;			//ライトビュープロジェクション行列。
};

//ライト用の定数バッファ。
cbuffer LightCB : register(b1) {

	DirectionLig dirLig;				//ディレクションライト用の構造体。
	float3 eyePos[3];					//視点の位置。
	PointLig ptLig[MAX_POINT_LIGHT];	//ポイントライト用の構造体。
	SpotLig spLig[MAX_SPOT_LIGHT];		//スポットライト用の構造体。
	ShadowCB shadow;					//シャドウ用の構造体。
    float4x4 mViewProjInv[3];      		//ビュープロジェクション行列の逆行列。
	int ptNum;							//ポイントライトの数。
	int spNum;							//スポットライトの数。
}