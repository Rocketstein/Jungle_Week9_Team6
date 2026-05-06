#include "CurveFloat.h"

IMPLEMENT_CLASS(UCurveFloat, UObject)

float UCurveFloat::Evaluate(float NormalizedT) const {
	NormalizedT = NormalizedT >= 0 ? NormalizedT : 0;
	NormalizedT = NormalizedT <= 1 ? NormalizedT : 1;
	uint64 T = static_cast<uint64>(NormalizedT * Curve.size());


	return 0;
}