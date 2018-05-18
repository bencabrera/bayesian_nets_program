#pragma once

#include "../../bitvec.h"

#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <functional>
#include <memory>

enum MatrixType {
	DYNAMIC,
	F,
	ONE_B,
	TERMINATOR,
	ZERO
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

		virtual ~Matrix();

		// type of matrix n -> m
		const Index n;
		const Index m;

		const MatrixType type;

		bool is_stochastic;

		Matrix(const Index n, const Index m, const MatrixType type, bool is_stochastic);

		virtual double get(const BitVec& to, const BitVec& from) const = 0;
		virtual void set(const BitVec& to, const BitVec& from, double val) = 0;
		virtual void add(const BitVec& to, const BitVec& from, double val) = 0;

		static BitVec init_one_mask(Index n);

		BitVec one_mask_n;
		BitVec one_mask_m;

	protected: 

	private:
};

using MatrixPtr = std::shared_ptr<Matrix>;

bool is_stochastic(const Matrix& m);


class DynamicMatrix : public Matrix {

	std::unordered_map<std::pair<BitVec,BitVec>, double> data;

	public: 

	DynamicMatrix(const Index n, const Index m);
	virtual ~DynamicMatrix();

	double get(const BitVec& to, const BitVec& from) const;
	void set(const BitVec& to, const BitVec& from, double val);
	void add(const BitVec& to, const BitVec& from, double val);
};

class FMatrix : public Matrix {
	public:
		const Index k;
		const bool b;

		FMatrix(const Index k, const bool b);
		virtual ~FMatrix();

		double get(const BitVec& to, const BitVec& from) const;
		void set(const BitVec& to, const BitVec& from, double val);
		void add(const BitVec& to, const BitVec& from, double val);
};

class OneBMatrix : public Matrix {
	public:
		const bool b;

		OneBMatrix(const bool b);
		virtual ~OneBMatrix();

		double get(const BitVec& to, const BitVec& from) const;
		void set(const BitVec& to, const BitVec& from, double val);
		void add(const BitVec& to, const BitVec& from, double val);
};

class TerminatorMatrix : public Matrix {
	public:
		TerminatorMatrix();
		virtual ~TerminatorMatrix();

		double get(const BitVec& to, const BitVec& from) const;
		void set(const BitVec& to, const BitVec& from, double val);
		void add(const BitVec& to, const BitVec& from, double val);
};

class ZeroMatrix : public Matrix {
	public:
		ZeroMatrix(const Index n, const Index m);
		virtual ~ZeroMatrix();

		double get(const BitVec& to, const BitVec& from) const;
		void set(const BitVec& to, const BitVec& from, double val);
		void add(const BitVec& to, const BitVec& from, double val);
};
