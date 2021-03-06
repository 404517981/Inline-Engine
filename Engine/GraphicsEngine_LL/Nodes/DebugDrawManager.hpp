#pragma once

#include "../PerspectiveCamera.hpp"
#include "GraphicsApi_LL/IGxapiManager.hpp"

#include <memory>
#include <optional>

namespace inl::gxeng {


class DebugObject {
public:
	virtual ~DebugObject() = default;

	virtual void GetMesh(std::vector<Vec3>& vertices, std::vector<uint32_t>& indices) const = 0;

	unsigned GetStride() const {
		return sizeof(Vec3);
	}

	int GetLife() const {
		return m_life;
	}

	void SetLife(int life) {
		m_life = life;
	}

	bool IsAlive() const {
		return m_life >= 0;
	}

	Vec3 GetColor() const {
		return m_color;
	}

	void SetColor(Vec3 newColor) {
		m_color = newColor;
	}

protected:
	int m_life;
	Vec3 m_color;
};


class DebugSphere : public DebugObject {
	Vec4 d; //w=radius

	static void GetStaticMesh(std::vector<Vec3>& vertices, std::vector<uint32_t>& indices) {
		static std::vector<Vec3> staticVertices;
		static std::vector<uint32_t> staticIndices;
		static bool is_init = false;

		if (!is_init) {
			//LINES
			const float pi = 3.14159265f;

			const int resolution_u = 10;
			const int resolution_v = 10 * 2;

			const float u_step = 1.f / resolution_u;
			const float v_step = 1.f / resolution_v;

			float u = 0;
			float v = 0;
			for (int u_index = 0; u_index < resolution_u; u_index += 1, u += u_step) {
				//0 <= alpha <= pi
				const float alpha = u * pi;

				for (int v_index = 0; v_index < resolution_v; v_index += 1, v += v_step) {
					//0 <= theta <= 2*pi
					const float theta = v * 2 * pi;
					staticVertices.push_back(Vec3(cosf(alpha) * cosf(theta), sinf(alpha) * cosf(theta), sinf(theta)));
				}
			}

			int curr_index = 0;
			for (int u_index = 0; u_index < resolution_u; u_index += 1, u += u_step) {
				for (int v_index = 0; v_index < resolution_v; v_index += 1, v += v_step) {
					//if there is a preceding vertex
					if (v_index - 1 >= 0) {
						staticIndices.push_back(curr_index);
					}

					staticIndices.push_back(curr_index);

					curr_index += 1;
				}

				staticIndices.push_back(curr_index - resolution_v);
			}

			staticVertices.shrink_to_fit();
			staticIndices.shrink_to_fit();

			is_init = true;
		}

		vertices = staticVertices;
		indices = staticIndices;
	}

public:
	DebugSphere(Vec3 pos, float radius, int newLife, Vec3 newColor = Vec3(1.0f, 1.0f, 1.0f)) {
		d = Vec4(pos, radius);
		m_life = newLife;
		m_color = newColor;
	}

	void GetMesh(std::vector<Vec3>& vertices, std::vector<uint32_t>& indices) const override {
		GetStaticMesh(vertices, indices);

		for (int c = 0; c < vertices.size(); ++c) {
			vertices[c] = vertices[c] * d.w + d.xyz;
		}
	}
};


class DebugCross : public DebugObject {
	Vec4 d; //w=size

public:
	DebugCross(Vec3 pos, float size, int newLife, Vec3 newColor = Vec3(1.0f, 1.0f, 1.0f)) {
		d = Vec4(pos, size);
		m_life = newLife;
		m_color = newColor;
	}

	void GetMesh(std::vector<Vec3>& vertices, std::vector<uint32_t>& indices) const override {
		//LINES
		vertices.clear();
		indices.clear();
		vertices.reserve(4);
		indices.reserve(4);

		vertices.push_back(d.xyz.ToVector() - Vec3(1.f, 0.f, 0.f) * d.w);
		vertices.push_back(d.xyz.ToVector() + Vec3(1.f, 0.f, 0.f) * d.w);

		vertices.push_back(d.xyz.ToVector() - Vec3(0.f, 1.f, 0.f) * d.w);
		vertices.push_back(d.xyz.ToVector() + Vec3(0.f, 1.f, 0.f) * d.w);

		vertices.push_back(d.xyz.ToVector() - Vec3(0.f, 0.f, 1.f) * d.w);
		vertices.push_back(d.xyz.ToVector() + Vec3(0.f, 0.f, 1.f) * d.w);

		indices.push_back(0);
		indices.push_back(1);

		indices.push_back(2);
		indices.push_back(3);

		indices.push_back(4);
		indices.push_back(5);
	}
};


class DebugLine : public DebugObject {
	Vec3 s, e;

public:
	DebugLine(Vec3 start, Vec3 end, int newLife, Vec3 newColor = Vec3(1.0f, 1.0f, 1.0f)) {
		s = start;
		e = end;
		m_life = newLife;
		m_color = newColor;
	}

	void GetMesh(std::vector<Vec3>& vertices, std::vector<uint32_t>& indices) const override {
		//LINES
		vertices.clear();
		indices.clear();
		vertices.reserve(2);
		indices.reserve(2);

		vertices.push_back(s);
		vertices.push_back(e);
		indices.push_back(0);
		indices.push_back(1);
	}
};


class DebugBox : public DebugObject{
	Vec3 min, max;

public:
	DebugBox(Vec3 newMin, Vec3 newMax, int newLife, Vec3 newColor = Vec3(1.0f, 1.0f, 1.0f)) {
		min = newMin;
		max = newMax;
		m_life = newLife;
		m_color = newColor;
	}

	void GetMesh(std::vector<Vec3>& vertices, std::vector<uint32_t>& indices) const override {
		//LINES
		vertices.clear();
		indices.clear();
		vertices.reserve(8);
		indices.reserve(24);

		std::vector<Vec3> min_max;
		min_max.push_back(min);
		min_max.push_back(max);

		vertices.push_back(Vec3(min_max[0].x, min_max[0].y, min_max[0].z));
		vertices.push_back(Vec3(min_max[0].x, min_max[0].y, min_max[1].z));
		vertices.push_back(Vec3(min_max[0].x, min_max[1].y, min_max[0].z));
		vertices.push_back(Vec3(min_max[0].x, min_max[1].y, min_max[1].z));
		vertices.push_back(Vec3(min_max[1].x, min_max[0].y, min_max[0].z));
		vertices.push_back(Vec3(min_max[1].x, min_max[0].y, min_max[1].z));
		vertices.push_back(Vec3(min_max[1].x, min_max[1].y, min_max[0].z));
		vertices.push_back(Vec3(min_max[1].x, min_max[1].y, min_max[1].z));

		indices.push_back(0); indices.push_back(1);
		indices.push_back(0); indices.push_back(2);
		indices.push_back(0); indices.push_back(4);
		indices.push_back(1); indices.push_back(3);
		indices.push_back(1); indices.push_back(5);
		indices.push_back(2); indices.push_back(3);
		indices.push_back(2); indices.push_back(6);
		indices.push_back(3); indices.push_back(7);
		indices.push_back(4); indices.push_back(5);
		indices.push_back(4); indices.push_back(6);
		indices.push_back(5); indices.push_back(7);
		indices.push_back(6); indices.push_back(7);
	}
};


class DebugFrustum : public DebugObject{
	Vec3 nearLowerLeft, nearUpperLeft, nearLowerRight, farLowerLeft, farUpperLeft, farLowerRight;

public:
	DebugFrustum(Vec3 newNearLowerLeft,
		Vec3 newNearUpperLeft,
		Vec3 newNearLowerRight,
		Vec3 newFarLowerLeft,
		Vec3 newFarUpperLeft,
		Vec3 newFarLowerRight,
		int newLife,
		Vec3 newColor = Vec3(1.0f, 1.0f, 1.0f)) {
		nearLowerLeft = newNearLowerLeft;
		nearUpperLeft = newNearUpperLeft;
		nearLowerRight = newNearLowerRight;
		farLowerLeft = newFarLowerLeft;
		farUpperLeft = newFarUpperLeft;
		farLowerRight = newFarLowerRight;
		m_life = newLife;
		m_color = newColor;
	}

	void GetMesh(std::vector<Vec3>& vertices, std::vector<uint32_t>& indices) const override {
		//LINES
		vertices.clear();
		indices.clear();
		vertices.reserve(8);
		indices.reserve(24);

		vertices.push_back(nearLowerLeft);
		vertices.push_back(nearLowerRight);
		vertices.push_back(nearLowerRight + (nearUpperLeft - nearLowerLeft));
		vertices.push_back(nearUpperLeft);

		vertices.push_back(farLowerLeft);
		vertices.push_back(farLowerRight);
		vertices.push_back(farLowerRight + (farUpperLeft - farLowerLeft));
		vertices.push_back(farUpperLeft);

		//LINES
		uint32_t starting_index = 0;
		uint32_t ending_index = 3;
		uint32_t curr_index = starting_index;
		for (int i = 0; i < 2; i += 1) {
			for (int j = 0; j < 4; j += 1) {
				indices.push_back(curr_index);
				indices.push_back((curr_index == ending_index) ? starting_index : ++curr_index);
			}
			curr_index = starting_index = 4;
			ending_index = 7;
		}

		curr_index = 0;
		for (int i = 0; i < 4; i += 1) {
			indices.push_back(curr_index);
			indices.push_back(curr_index + 4);
			curr_index += 1;
		}
	}
};



/// <summary>
/// Manages all debug objects
/// </summary>
class DebugDrawManager {
public:
	static DebugDrawManager& GetInstance() {
		static DebugDrawManager ddm;
		return ddm;
	}

	void Update() {
		for (int c = 0; c < m_objects.size(); ++c) {
			m_objects[c]->SetLife((m_objects[c]->GetLife()) - 1);
			if (!m_objects[c]->IsAlive()) {
				m_objects[c].reset();
			}
		}
	}

	void AddSphere(Vec3 pos, float radius, int life, Vec3 newColor = Vec3(1.0f, 1.0f, 1.0f)) {
		for (int c = 0; c < m_objects.size(); ++c) {
			if (!m_objects[c]) {
				m_objects[c] = std::make_unique<DebugSphere>(pos, radius, life + 1, newColor);
				return;
			}
		}

		m_objects.push_back(std::make_unique<DebugSphere>(pos, radius, life + 1, newColor));
	}

	void AddCross(Vec3 pos, float size, int life, Vec3 newColor = Vec3(1.0f, 1.0f, 1.0f)) {
		for (int c = 0; c < m_objects.size(); ++c) {
			if (!m_objects[c]) {
				m_objects[c] = std::make_unique<DebugCross>(pos, size, life + 1, newColor);
				return;
			}
		}

		m_objects.push_back(std::make_unique<DebugCross>(pos, size, life + 1, newColor));
	}

	void AddLine(Vec3 start, Vec3 end, int life, Vec3 newColor = Vec3(1.0f, 1.0f, 1.0f)) {
		for (int c = 0; c < m_objects.size(); ++c) {
			if (!m_objects[c]) {
				m_objects[c] = std::make_unique<DebugLine>(start, end, life + 1, newColor);
				return;
			}
		}

		m_objects.push_back(std::make_unique<DebugLine>(start, end, life + 1, newColor));
	}

	void AddBox(Vec3 min, Vec3 max, int life, Vec3 newColor = Vec3(1.0f, 1.0f, 1.0f)) {
		for (int c = 0; c < m_objects.size(); ++c) {
			if (!m_objects[c]) {
				m_objects[c] = std::make_unique<DebugBox>(min, max, life + 1, newColor);
				return;
			}
		}

		m_objects.push_back(std::make_unique<DebugBox>(min, max, life + 1, newColor));
	}

	void AddFrustum(Vec3 newNearLowerLeft,
		Vec3 newNearUpperLeft,
		Vec3 newNearLowerRight,
		Vec3 newFarLowerLeft,
		Vec3 newFarUpperLeft,
		Vec3 newFarLowerRight,
		int life,
		Vec3 newColor = Vec3(1.0f, 1.0f, 1.0f)) {
		for (int c = 0; c < m_objects.size(); ++c) {
			if (!m_objects[c]) {
				m_objects[c] = std::make_unique<DebugFrustum>(
					newNearLowerLeft,
					newNearUpperLeft,
					newNearLowerRight,
					newFarLowerLeft,
					newFarUpperLeft,
					newFarLowerRight,
					life + 1,
					newColor);
				return;
			}
		}

		m_objects.push_back(std::make_unique<DebugFrustum>(
			newNearLowerLeft,
			newNearUpperLeft,
			newNearLowerRight,
			newFarLowerLeft,
			newFarUpperLeft,
			newFarLowerRight,
			life + 1,
			newColor));
	}

	const std::vector<std::shared_ptr<DebugObject>>& GetObjects() const {
		return m_objects;
	}

private:
	std::vector<std::shared_ptr<DebugObject>> m_objects;

private:
	DebugDrawManager() {}
	DebugDrawManager(const DebugDrawManager&);
	void operator=(const DebugDrawManager&);
};


} // namespace inl::gxeng

