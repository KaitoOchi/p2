////////////////////////////////////////////////
// フォワードライティングのレジスタ。
////////////////////////////////////////////////
Texture2D<float4>           g_albedoMap : register(t0);			    //アルベドマップ。
Texture2D<float4>           g_normalMap : register(t1);			    //ノーマルマップ。
Texture2D<float4>           g_metallicSmoothMap : register(t2);     //メタリックスムースマップ。rにメタリック、aに滑らかさ。
Texture2D<float4>           g_zPrepass : register(t10);			    //ZPrepass。
Texture2D<float4>           g_shadowMap : register(t11);			//シャドウマップ。
StructuredBuffer<uint>      pointLightListInTile : register(t12);    //ポイントライト用のUAV。
StructuredBuffer<uint>      spotLightListInTile : register(t13);    //スポットライト用のUAV。
sampler                     g_sampler : register(s0);				//サンプラステート。