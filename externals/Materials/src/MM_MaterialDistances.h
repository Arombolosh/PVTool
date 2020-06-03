#ifndef MM_MaterialDistanceH
#define MM_MaterialDistanceH

#include <vector>

#include "MM_Utils.h"

namespace MM {


	/*! \brief Class that is used to determine the distance (similarity) between two materials. */
	class MaterialDistance {
	public:
		/*! Constructor for creating a instance.*/
		MaterialDistance(MaterialFile* mat, double dist) :
			m_material(mat),
			m_distance(dist)
		{}

		/*! Pointer to one of the two materials used for comparison. */
		MaterialFile*	m_material;

		/*! Weighted sum of differences between basic material parameters. */
		double			m_distance;

		friend bool operator<(const MaterialDistance& lhs, const MaterialDistance& rhs) {
			return lhs.m_distance < rhs.m_distance;
		}

		friend bool operator>(const MaterialDistance& lhs, const MaterialDistance& rhs) {
			return lhs.m_distance > rhs.m_distance;
		}

		friend bool operator==(const MaterialDistance& lhs, const MaterialDistance& rhs) {
			return lhs.m_distance == rhs.m_distance;
		}
	};

	/*! \brief Class for handling a simiarity list of materials.
		It holds a vector of MaterialDistance objects sorted by their distance.
	*/
	class MaterialDistances {
	public:
		/*! Standard constructor.
			As default only basic parameters will be tested.
			\param mat Pointer to reference material. This pointer must be valid over the whole livetime of this class.
		*/
		MaterialDistances(MaterialFile* mat);

		/*! Add a material to the vector.*/
		void add(MaterialFile* mat);

		/*! Creates the vector from the give MaterialFile vector.
		*/
		void set(const std::vector<MM::MaterialFile*>& matVect);

		/*! Creates the vector from the give MaterialFile vector.
		*/
		void set(std::vector<MM::MaterialFile>& matVect);

		/*! Sets the onlyDelphin flag. A change of this flag will envoke a refresh.
			If the flag is true the moisture retention function will be tested additionally to basic parameters.
		*/
		void setOnlyDelphinFlag(bool onlyDelphinReady);

		/*! Clears the internal vector.*/
		void clear();

		/*! Returns true if the internal material vector is empty.*/
		bool empty() const;

		/*! Returns the size of the internal material vector.*/
		unsigned int size() const;

		/*! Constant index operator.*/
		const MaterialDistance& operator[](unsigned int index) const;

		/*! Returns the reference material file.*/
		 MaterialFile const * baseMaterialFile() const;

		 /*! Returns the reference material file.*/
		  MaterialFile* baseMaterialFile();

	private:
		/*! Refreshes the distances and creates a new vector.
			This is necessary if m_onlyDelphinReady flag is changed.
		*/
		void refresh();

		MaterialFile*					m_baseMaterialFile;
		std::vector<MaterialDistance>	m_distances;
		UsedParameterForDistance		m_usedPara;
	};

} // namespace MM

#endif // MM_MaterialDistanceH
