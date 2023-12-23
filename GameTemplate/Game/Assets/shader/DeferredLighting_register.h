////////////////////////////////////////////////
// ディファードライティングのレジスタ。
////////////////////////////////////////////////
static const int PORTAL_NUM = 2;

Texture2D<float4> g_albedoTexture                   : register(t0);     //アルベドカラー。
Texture2D<float4> g_normalTexture                   : register(t1);     //法線マップ。
Texture2D<float4> g_metallicShadowSmoothTexture     : register(t2);     //メタリックスムース。
Texture2D<float4> g_param                           : register(t3);     //パラメータ。
Texture2D<float4> g_shadowMap                       : register(t4);     //シャドウマップ
Texture2D<float4> g_portalRenderTexture[PORTAL_NUM] : register(t5);     //ポータルテクスチャ。
StructuredBuffer<uint> pointLightListInTile         : register(t20);    //ポイントライト用のUAV。
StructuredBuffer<uint> spotLightListInTile          : register(t21);    //スポットライト用のUAV。
sampler           g_sampler                         : register(s0);		//サンプラ。