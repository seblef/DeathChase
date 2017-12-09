
#ifndef _PARTICLERENDERERSTD_H_
#define _PARTICLERENDERERSTD_H_

#include "IParticleRenderer.h"
#include "TDynamicTab.h"
#include "TMemoryAllocator.h"
#include "Vertex.h"
#include "Handles.h"

namespace Core
{

class ParticleEngine;

class ParticleRenderer_Point : public IParticleRenderer
{
	public:

		ParticleRenderer_Point(bool bInstance=false);
		~ParticleRenderer_Point();

		void Draw(Particle *pParticles, int iCount, Camera *pCamera);
		bool Parse(ScriptFile *pFile) { return true; }

		void Clear() {}

		IParticleRenderer *New();
		void Delete() { m_sAllocator.Delete(this); }

	protected:

		TDynamicTab<VertexPD> m_Tab;
		HMaterial m_hMaterial;

		bool m_bInstance;

		friend class ParticleEngine;
		static TMemoryAllocator<ParticleRenderer_Point> m_sAllocator;
};

class ParticleRenderer_Quad : public IParticleRenderer
{
	public:

		ParticleRenderer_Quad(bool bInstance=false) : m_fSize(1.0f),m_bInstance(bInstance) {}
		~ParticleRenderer_Quad();

		void Draw(Particle *pParticles, int iCount, Camera *pCamera);
		bool Parse(ScriptFile *pFile);

		void SetSize(float fSize) { m_fSize=fSize; }
		float GetSize() const { return m_fSize; }

		void SetMaterial(HMaterial hMat);
		HMaterial GetMaterial() const { return m_hMaterial; }

		void Clear();

		IParticleRenderer *New();
		void Delete() { m_sAllocator.Delete(this); }

	protected:

		TDynamicTab<VertexPDT> m_Vertices;
		TDynamicTab<uint16> m_Indices;

		float m_fSize;
		HMaterial m_hMaterial;

		bool m_bInstance;

		friend class ParticleEngine;
		static TMemoryAllocator<ParticleRenderer_Quad> m_sAllocator;
};

class ParticleRenderer_Mesh : public IParticleRenderer
{
	public:

		ParticleRenderer_Mesh(bool bInstance=false) : m_fSize(1.0f), m_bInstance(bInstance) {}
		~ParticleRenderer_Mesh();

		void Draw(Particle *pParticles, int iCount, Camera *pCamera);
		bool Parse(ScriptFile *pFile);

		void SetSize(float fSize) { m_fSize=fSize; }
		float GetSize() const { return m_fSize; }

		void SetMesh(HMesh hMesh);
		HMesh GetMesh() const { return m_hMesh; }

		void Clear();

		IParticleRenderer *New();
		void Delete() { m_sAllocator.Delete(this); }

	protected:

		float m_fSize;
		HMesh m_hMesh;

		bool m_bInstance;

		friend class ParticleEngine;
		static TMemoryAllocator<ParticleRenderer_Mesh> m_sAllocator;
};

}

#endif