#pragma once

#include "imgui.h"
#include "ImCurveEdit.h"
#include "Math/CurveFloat.h"
#include "Math/Vector.h"

#include <algorithm>

namespace CurveEditor
{
	class FCurveFloatEditDelegate : public ImCurveEdit::Delegate
	{
	public:
		explicit FCurveFloatEditDelegate(UCurveFloat& InCurve)
			: Curve(InCurve)
		{
			if (Curve.Curve.empty())
			{
				Curve.ResetLinear();
			}

			ViewMin = ImVec2(0.0f, 0.0f);
			ViewMax = ImVec2(1.0f, 1.0f);
			for (const FVector2& Key : Curve.Curve)
			{
				ViewMin.y = (std::min)(ViewMin.y, Key.Y);
				ViewMax.y = (std::max)(ViewMax.y, Key.Y);
			}
			if (ViewMax.y - ViewMin.y < 1e-4f)
			{
				ViewMax.y = ViewMin.y + 1.0f;
			}
		}

		size_t GetCurveCount() override { return 1; }
		ImCurveEdit::CurveType GetCurveType(size_t) const override { return ImCurveEdit::CurveLinear; }
		ImVec2& GetMin() override { return ViewMin; }
		ImVec2& GetMax() override { return ViewMax; }
		size_t GetPointCount(size_t) override { return Curve.Curve.size(); }
		uint32_t GetCurveColor(size_t) override { return 0xFF48B8FF; }
		unsigned int GetBackgroundColor() override { return 0xFF161616; }

		ImVec2* GetPoints(size_t) override
		{
			return Curve.Curve.empty() ? nullptr : reinterpret_cast<ImVec2*>(Curve.Curve.data());
		}

		int EditPoint(size_t, int PointIndex, ImVec2 Value) override
		{
			if (PointIndex < 0 || PointIndex >= static_cast<int>(Curve.Curve.size()))
			{
				return PointIndex;
			}

			Value.x = (std::max)(0.0f, (std::min)(1.0f, Value.x));
			Curve.Curve[PointIndex] = FVector2(Value.x, Value.y);
			Curve.SortKeys();

			for (int32 Index = 0; Index < static_cast<int32>(Curve.Curve.size()); ++Index)
			{
				if (Curve.Curve[Index].X == Value.x && Curve.Curve[Index].Y == Value.y)
				{
					return Index;
				}
			}
			return PointIndex;
		}

		void AddPoint(size_t, ImVec2 Value) override
		{
			Curve.AddKey(Value.x, Value.y);
		}

	private:
		UCurveFloat& Curve;
		ImVec2 ViewMin;
		ImVec2 ViewMax;
	};

	inline bool DrawCurveFloat(const char* Label, UCurveFloat* Curve, const ImVec2& Size = ImVec2(0.0f, 140.0f))
	{
		if (!Curve)
		{
			ImGui::TextDisabled("%s: None", Label);
			return false;
		}

		bool bChanged = false;
		ImGui::TextUnformatted(Label);

		FCurveFloatEditDelegate Delegate(*Curve);
		const float Width = Size.x > 0.0f ? Size.x : ImGui::GetContentRegionAvail().x;
		const int Result = ImCurveEdit::Edit(Delegate, ImVec2(Width, Size.y), ImGui::GetID("##CurveFloat"));
		bChanged |= Result != 0;

		if (ImGui::SmallButton("+ Key"))
		{
			Curve->AddKey(0.5f, Curve->Evaluate(0.5f));
			bChanged = true;
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Linear"))
		{
			Curve->ResetLinear();
			bChanged = true;
		}
		ImGui::SameLine();
		ImGui::TextDisabled("%d keys", static_cast<int>(Curve->Curve.size()));

		return bChanged;
	}
}
