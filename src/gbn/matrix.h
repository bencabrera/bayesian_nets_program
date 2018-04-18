#include "../bitvec.h"

#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <functional>

enum MatrixType {
	DYNAMIC,
	F,
	IDENTITY,
	ZEROS,
	ONE_B
};

namespace std {
	template<>
	struct hash<std::pair<BitVec, BitVec>> {
		std::size_t operator()(std::pair<BitVec, BitVec> const &p) const {
			std::size_t seed1(0);

			boost::hash_combine(seed1, std::hash<BitVec>{}(p.first));
			boost::hash_combine(seed1, std::hash<BitVec>{}(p.second));

			return seed1;
		}
	};
}

class Matrix {
	public:

		// type of matrix n -> m
		const int n;
		const int m;

		const MatrixType type;

		Matrix(const int n, const int m, const MatrixType type);

		virtual double get(const BitVec& to, const BitVec& from) const = 0;
		virtual double& set(const BitVec& to, const BitVec& from, double val) = 0;

		// private: 
		// const BitVec one_mask_n;
		// const BitVec one_mask_m;

		// static BitVec init_one_mask(int n) {
		// BitVec b;
		// for(int i = n; i < MAX_PLACES; i++)
		// b[i] = 1;

		// return b;
		// }
};


class DynamicMatrix : public Matrix {

	std::unordered_map<std::pair<BitVec,BitVec>, double> data;

	public: 

	DynamicMatrix(const int n, const int m);

	double get(const BitVec& to, const BitVec& from) const;
	double& set(const BitVec& to, const BitVec& from, double val);
};

class FMatrix : public Matrix {
	public:

		const int k;
		const bool b;


		FMatrix(const int n, const int m, const bool b);

		double get(const BitVec& to, const BitVec& from) const;
		double& set(const BitVec& to, const BitVec& from, double val);
};

class OneBMatrix : public Matrix {
	public:

		const bool b;

		OneBMatrix(const bool b);

		double get(const BitVec& to, const BitVec& from) const;
		double& set(const BitVec& to, const BitVec& from, double val);
};
