////////////////////////////////////////////////
// シェーダー関数。
////////////////////////////////////////////////
//ディザパターン。
static const int DITHER_PATTERN[4][4] = {
    { 0, 32,  8, 40},
    { 48, 16, 56, 24},
    { 12, 44,  4, 36},
    { 60, 28, 52, 20},
};

static const int DITHER_CLIP_RANGE = 80.0f;		//ディザリングのクリップ範囲。
static const int DITHER_CLIP_RATE = 10.0f;		//ディザリングのクリップ割合。

// 近傍8テクセルへのUVオフセット。
static const float2 uvOffset[8] = {
    float2(           0.0f,  1.0f / 900.0f), //上
    float2(           0.0f, -1.0f / 900.0f), //下
    float2( 1.0f / 1600.0f,           0.0f), //右
    float2(-1.0f / 1600.0f,           0.0f), //左
    float2( 1.0f / 1600.0f,  1.0f / 900.0f), //右上
    float2(-1.0f / 1600.0f,  1.0f / 900.0f), //左上
    float2( 1.0f / 1600.0f, -1.0f / 900.0f), //右下
    float2(-1.0f / 1600.0f, -1.0f / 900.0f)  //左下
};

////////////////////////////////////////////////
// 関数。
////////////////////////////////////////////////
/// <summary>
/// ディザリングを計算。
/// </summary>
/// <param name="pos">頂点座標</param>
/// <param name="distToEye">カメラとの距離</param>
void Dithering(float2 pos, float distToEye)
{
	//スクリーン座標系でのxy座標を4で割った余りを求める。
	int x = (int)fmod(pos.x, 4.0f);
	int y = (int)fmod(pos.y, 4.0f);

	//上で求めたxyを利用して、このピクセルのディザリング閾値を取得する。
	int dither = DITHER_PATTERN[y][x];

	//視点からクリップ範囲までの距離を計算する。
	float eyeToClipRange = max(0.0f, distToEye - DITHER_CLIP_RANGE);

	//0～1の範囲でクリップ率を求める。
	float clipRate = 1.0f - min(1.0f, eyeToClipRange / DITHER_CLIP_RATE);

	clip(dither - 64 * clipRate);
}

/// <summary>
/// 輪郭線を計算。
/// </summary>
/// <param name="posInProj">頂点の正規化スクリーン座標系</param>
/// <param name="depthTexture">深度テクスチャ</param>
/// <param name="Sampler">サンプラー</param>
/// <param name="albedo">元のカラー</param>
float4 Outline(
    float4 posInProj, 
    Texture2D<float4> depthTexture,
    sampler Sampler,
    float4 albedo
) {
    // 近傍8テクセルの深度値を計算して、エッジを抽出する
    // 正規化スクリーン座標系からUV座標系に変換する
    float2 uv = posInProj.xy * float2(0.5f, -0.5f) + 0.5f;

    // このピクセルの深度値を取得
    float depth = depthTexture.Sample(Sampler, uv).z;

    // 近傍8テクセルの深度値の平均値を計算する
    float depth2 = 0.0f;
    for( int i = 0; i < 8; i++)
    {
        depth2 += depthTexture.Sample(Sampler, uv + uvOffset[i]).z;
    }
    depth2 /= 8.0f;

    // 自身の深度値と近傍8テクセルの深度値の差を調べる
    if(abs(depth - depth2) > 0.0001f)
    {
        // 深度値が結構違う場合はピクセルカラーを変える。
        return float4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    // 普通のテクスチャ
    return albedo;
}