////////////////////////////////////////////////
// シェーダー関数。
////////////////////////////////////////////////
//ディザパターン
static const int pattern[4][4] = {
    { 0, 32,  8, 40},
    { 48, 16, 56, 24},
    { 12, 44,  4, 36},
    { 60, 28, 52, 20},
};


////////////////////////////////////////////////
// 関数。
////////////////////////////////////////////////
/// <summary>
/// ディザリングを計算
/// </summary>
void Dithering(float3 pos)
{
	//スクリーン座標系でのxy座標を4で割った余りを求める
	int x = (int)fmod(pos.x, 4.0f);
	int y = (int)fmod(pos.y, 4.0f);

	//上で求めたxyを利用して、このピクセルのディザリング閾値を取得する
	int dither = pattern[y][x];

	//50以下ならピクセルキル
	clip(dither - 50);
}