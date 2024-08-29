
#include "panels/ControllerView.h"
#include "InputManager.h"
#include "core/ServiceLocator.h"
#include "glviewer/unproject.h"
#include "ImGuizmo.h"
#include <iostream>

class  Ray
{
public:
	/// Construct a degenerate ray with zero origin and direction.
	Ray() noexcept = default;

	/// Construct from origin and direction. The direction will be normalized.
	Ray(const vec3& origin, const vec3& direction) noexcept
	{
		Define(origin, direction);
	}

	/// Copy-construct from another ray.
	Ray(const Ray& ray) noexcept = default;

	/// Assign from another ray.
	Ray& operator =(const Ray& rhs) noexcept = default;

	/// Check for equality with another ray.
	bool operator ==(const Ray& rhs) const { return origin_ == rhs.origin_ && direction_ == rhs.direction_; }

	/// Check for inequality with another ray.
	bool operator !=(const Ray& rhs) const { return origin_ != rhs.origin_ || direction_ != rhs.direction_; }

	/// Define from origin and direction. The direction will be normalized.
	void Define(const vec3& origin, const vec3& direction)
	{
		origin_ = origin;
		direction_ = direction;
		direction_.normalize();
	}

	/// Project a point on the ray.
	vec3 Project(const vec3& point) const
	{
		vec3 offset = point - origin_;
		return origin_ + offset.dot(direction_) * direction_;
	}

	/// Return distance of a point from the ray.
	float Distance(const vec3& point) const
	{
		vec3 projected = Project(point);
		return (point - projected).norm();
	}

	/// Return closest point to another ray.
	vec3 ClosestPoint(const Ray& ray) const {
		// Algorithm based on http://paulbourke.net/geometry/lineline3d/
		vec3 p13 = origin_ - ray.origin_;
		vec3 p43 = ray.direction_;
		vec3 p21 = direction_;

		float d1343 = p13.dot(p43);
		float d4321 = p43.dot(p21);
		float d1321 = p13.dot(p21);
		float d4343 = p43.dot(p43);
		float d2121 = p21.dot(p21);

		float d = d2121 * d4343 - d4321 * d4321;
		if (abs(d) < 0.000001f)
			return origin_;
		float n = d1343 * d4321 - d1321 * d4343;
		float a = n / d;

		return origin_ + a * direction_;
	}


	bool HitDistance(const vec3& center, const float& radius, float& dis) const {
		vec3 centeredOrigin = origin_ - center;
		float squaredRadius = radius * radius;

		// Check if ray originates inside the sphere
		if (centeredOrigin.squaredNorm() <= squaredRadius)
			return false;

		// Calculate intersection by quadratic equation
		float a = direction_.dot(direction_);
		float b = 2.0f * centeredOrigin.dot(direction_);
		float c = centeredOrigin.dot(centeredOrigin) - squaredRadius;
		float d = b * b - 4.0f * a * c;

		// No solution
		if (d < 0.0f)
			return false;

		// Get the nearer solution
		float dSqrt = sqrtf(d);
		float dist = (-b - dSqrt) / (2.0f * a);
		if (dist >= 0.0f)
			dis = dist;
		else
			dis = (-b + dSqrt) / (2.0f * a);

		return true;
	}


	/// Ray origin.
	vec3 origin_;
	/// Ray direction.
	vec3 direction_;
};
int IntersectionWithSphere(const Ray& ray, const float radius, const std::vector<vec3*>& arr) {
	float dis = std::numeric_limits<float>::infinity();
	int index = -1;
	for (int i = 0; i < arr.size(); i++) {
		float tm = 0.0f;
		if (ray.HitDistance(*arr[i], radius, tm)) {
			if (tm < dis) {
				index = i;
				dis = tm;
			}
		}
	}
	return index;
}
std::vector<vec3> arr = { vec3{0,0,0} };
namespace Geomerty {
	ControllerView::ControllerView(const std::string& p_title, bool p_opened, const UI::Settings::PanelWindowSettings& p_windowSettings) :AView(p_title, p_opened, p_windowSettings)
	{
		viewer.append_mesh();
		viewer.init();
		viewer.post_resize(800, 900);
		ResizeEvent += [&](int p_width, int p_height) {
			viewer.post_resize(p_width, p_height);
			};

	}
	void ControllerView::Update(float p_deltaTime)
	{
		AView::Update(p_deltaTime);

		//Intersection(arr);
		if (IsHovered() && !ImGui::IsKeyDown(ImGuiKey_::ImGuiKey_Space)) {
			auto& inputManager = Geomerty::ServiceLocator::Get<Windowing::Inputs::InputManager>();
			viewer.mouse_scroll(inputManager.GetMouseScrollOffset());
			if (inputManager.IsMouseButtonPressed(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_LEFT))
				viewer.mouse_down(Geomerty::Viewer::MouseButton::Left);
			if (inputManager.IsMouseButtonReleased(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_LEFT))
				viewer.mouse_up(Geomerty::Viewer::MouseButton::Left);
			if (inputManager.IsMouseButtonPressed(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_MIDDLE))
				viewer.mouse_down(Geomerty::Viewer::MouseButton::Middle);
			if (inputManager.IsMouseButtonReleased(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_MIDDLE))
				viewer.mouse_up(Geomerty::Viewer::MouseButton::Middle);
			if (inputManager.IsMouseButtonPressed(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
				viewer.mouse_down(Geomerty::Viewer::MouseButton::Right);
			if (inputManager.IsMouseButtonReleased(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
				viewer.mouse_up(Geomerty::Viewer::MouseButton::Right);
			auto [x, y] = inputManager.GetMousePosition();
			auto [lx, ly] = GetPosition();
			viewer.mouse_move(x - lx, y - ly - 25.0);
		}

	}
	//to do :split
	void ControllerView::Intersection(std::vector<vec3*>& pos_arr)
	{
		auto mpos = ImGui::GetMousePos();
		auto [lx, ly] = GetPosition();
		float rx = mpos.x - lx;
		float ry = mpos.y - ly - 25;
		const Eigen::Vector2f pos(
			rx, viewer.core().viewport(3) - ry);
		auto [winWidth, winHeight] = GetSafeSize();

		const auto model = viewer.core().view;
		const auto proj = viewer.core().proj;
		const auto viewport = viewer.core().viewport;
		Eigen::RowVector3f src;
		Eigen::RowVector3f dir;
		Eigen::Matrix<float, 3, 1>win_s(pos(0, 0), pos(1, 0), 0);
		Eigen::Matrix<float, 3, 1>win_d(pos(0, 0), pos(1, 0), 1);
		Eigen::Matrix<float, 3, 1>d;
		Eigen::Vector3f src_f;
		Eigen::Vector3f dir_f;

		unproject(win_s, model, proj, viewport, src_f);
		unproject(win_d, model, proj, viewport, d);
		dir_f = d - src_f;
		src = src_f.transpose();
		dir = dir_f.transpose();
		Ray ray(src, dir);
		static int index = -1;
		if (ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right)) {
			index = IntersectionWithSphere(ray, 1.1f, pos_arr);
		}
		if (index != -1) {
			auto [winWidth, winHeight] = GetSafeSize();
			float xform[16] = {
			};
			float matrixRotation[3] = { 0,0,0 }, matrixScale[3] = { 1,1,1 };
			ImGuizmo::RecomposeMatrixFromComponents(pos_arr[index]->data(), matrixRotation, matrixScale, xform);
			ImGuizmo::SetRect(lx, ly + 25, winWidth, winHeight);
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::Manipulate(model.data(), proj.data(), ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, xform, NULL, NULL);
			ImGuizmo::DecomposeMatrixToComponents(xform, pos_arr[index]->data(), matrixRotation, matrixScale);
		}
	}
	void ControllerView::_Render_Impl()
	{
		Bind();
		viewer.draw();
		UnBind();
	}
	void ControllerView::_Draw_ImplInWindow()
	{

		//ImGui::Text("hellow");
		if (!arr.empty()) {
			Intersection(arr);
		}

	}
}