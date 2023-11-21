////////////////////////////////////////////////
// ディファードライティング用レジスタ。
////////////////////////////////////////////////
static const int PORTAL_NUM = 2;

Texture2D<float4> g_albedoTexture                   : register(t0);     //アルベド。
Texture2D<float4> g_normalTexture                   : register(t1);     //法線。
Texture2D<float4> g_metallicShadowSmoothTexture     : register(t2);     //メタリックスムース。
Texture2D<float4> g_param                           : register(t3);     //各種パラメータ。
Texture2D<float4> g_shadowMap                       : register(t4);     //シャドウマップ。
Texture2D<float4> g_portalRenderTexture[PORTAL_NUM] : register(t5);     //ポータル。
sampler           g_sampler                         : register(s0);		//サンプラー。