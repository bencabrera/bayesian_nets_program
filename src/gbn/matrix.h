#pragma once

#include "../bitvec.h"

#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <functional>
#include <memory>

enum MatrixType {
	DYNAMIC,
	F,
	IDENTITY,
	ZEROS,
	ONE_B,
	TERMINATOR
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

using Index = std::size_t;

class Matrix {
	public:

		// type of matrix n -> m
		const Index n;
		const Index m;

		const MatrixType type;

		Matrix(const Index n, const Index m, const MatrixType type);

		virtual double get(const BitVec& to, const BitVec& from) const = 0;
		virtual void set(const BitVec& to, const BitVec& from, double val) = 0;
		virtual void add(const BitVec& to, const BitVec& from, double val) = 0;

	protected: 
		const BitVec one_mask_n;
		const BitVec one_mask_m;

	private:
		static BitVec init_one_mask(Index n);
};

using MatrixPtr = std::shared_ptr<Matrix>;


class DynamicMatrix : public Matrix {

	std::unordered_map<std::pair<BitVec,BitVec>, double> data;

	public: 

	DynamicMatrix(const Index n, const Index m);

	double get(const BitVec& to, const BitVec& from) const;
	void set(const BitVec& to, const BitVec& from, double val);
	void add(const BitVec& to, const BitVec& from, double val);
};

class FMatrix : public Matrix {
	public:

		const Index k;
		const bool b;


		FMatrix(const Index n, const Index m, const bool b);

		double get(const BitVec& to, const BitVec& from) const;
		void set(const BitVec& to, const BitVec& from, double val);
		void add(const BitVec& to, const BitVec& from, double val);
};

class OneBMatrix : public Matrix {
	public:

		const bool b;

		OneBMatrix(const bool b);

		double get(const BitVec& to, const BitVec& from) const;
		void set(const BitVec& to, const BitVec& from, double val);
		void add(const BitVec& to, const BitVec& from, double val);
};

class TerminatorMatrix : public Matrix {
	public:
		TerminatorMatrix();

		double get(const BitVec& to, const BitVec& from) const;
		void set(const BitVec& to, const BitVec& from, double val);
		void add(const BitVec& to, const BitVec& from, double val);
};
bool is_stochastic(const Matrix& m);
