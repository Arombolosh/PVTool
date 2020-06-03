#include "MM_MaterialDistances.h"

namespace MM {

	MaterialDistances::MaterialDistances(MaterialFile* mat) :
		m_baseMaterialFile(mat),
		m_usedPara(UPFD_AllWithDELPHIN)
	{}


	void MaterialDistances::add(MaterialFile* mat) {
		double dist = materialDistance(m_baseMaterialFile->second, mat->second, m_usedPara);
		MaterialDistance tmp(mat, dist);
		if( m_distances.empty())
			m_distances.push_back(tmp);
		else {
			std::vector<MM::MaterialDistance>::iterator it = std::lower_bound(m_distances.begin(), m_distances.end(), tmp);
			m_distances.insert(it, tmp);
		}
	}

	void MaterialDistances::refresh() {
		if( m_distances.empty())
			return;
		std::vector<MM::MaterialDistance>::iterator it = m_distances.begin(), end = m_distances.end();
		for( ; it!=end; ++it) {
			double dist = materialDistance(m_baseMaterialFile->second, it->m_material->second, m_usedPara);
			it->m_distance = dist;
		}
		std::sort(m_distances.begin(), m_distances.end());
	}

	void MaterialDistances::set(const std::vector<MM::MaterialFile*>& matVect) {
		m_distances.clear();
		std::vector<MM::MaterialFile*>::const_iterator it = matVect.begin(), end = matVect.end();
		for( ; it!=end; ++it) {
			double dist = materialDistance(m_baseMaterialFile->second, (*it)->second, m_usedPara);
			m_distances.push_back(MaterialDistance(*it, dist));
		}
		std::sort(m_distances.begin(), m_distances.end());
	}

	void MaterialDistances::set(std::vector<MM::MaterialFile>& matVect) {
		m_distances.clear();
		std::vector<MM::MaterialFile>::iterator it = matVect.begin(), end = matVect.end();
		for( ; it!=end; ++it) {
			double dist = materialDistance(m_baseMaterialFile->second, it->second, m_usedPara);
			m_distances.push_back(MaterialDistance(&(*it), dist));
		}
		std::sort(m_distances.begin(), m_distances.end());
	}

	void MaterialDistances::setOnlyDelphinFlag(bool onlyDelphinReady) {
		UsedParameterForDistance usedPara = onlyDelphinReady ? UPFD_AllWithDELPHIN : UPFD_AllWithoutDELPHIN;
		if( m_usedPara != usedPara) {
			m_usedPara = usedPara;
			refresh();
		}
	}

	void MaterialDistances::clear() {
		m_distances.clear();
	}

	bool MaterialDistances::empty() const {
		return m_distances.empty();
	}

	unsigned int MaterialDistances::size() const {
		return m_distances.size();
	}

	const MaterialDistance& MaterialDistances::operator[](unsigned int index) const {
		return m_distances[index];
	}

	MaterialFile const * MaterialDistances::baseMaterialFile() const {
		return m_baseMaterialFile;
	}

	MaterialFile* MaterialDistances::baseMaterialFile() {
		return m_baseMaterialFile;
	}


} // namespace MM
