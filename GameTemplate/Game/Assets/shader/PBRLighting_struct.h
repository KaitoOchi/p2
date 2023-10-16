////////////////////////////////////////////////
// ライト用定数バッファの構造体。
////////////////////////////////////////////////

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
 };

/// <summary>
/// シャドウ用の構造体。
/// </summary>
struct ShadowCB
{
	float3 lightPos;		//ライトの座標。
	float4x4 mLVP;			//ライトビュープロジェクション行列。
};