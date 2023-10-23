////////////////////////////////////////////////
// PBRライティングの定数。
////////////////////////////////////////////////
static const float PI = 3.1415926f;         // 円周率。

////////////////////////////////////////////////
// グローバル変数。
////////////////////////////////////////////////
Texture2D<float4>           g_albedoMap : register(t0);			    //アルベドマップ。
Texture2D<float4>           g_normalMap : register(t1);			    //ノーマルマップ。
Texture2D<float4>           g_metallicSmoothMap : register(t2);     //メタリックスムースマップ。rにメタリック、aに滑らかさ。
Texture2D<float4>           g_shadowMap : register(t10);			//シャドウマップ。
sampler                     g_sampler : register(s0);				//サンプラステート。