#include "randomized_generation.h"


MatrixPtr generate_random_matrix(std::size_t n, std::size_t m, std::mt19937& mt, RandomMatrixParams params)
{
    std::uniform_real_distribution<double> rand_is_special_matrix(0,1);
    std::uniform_int_distribution<bool> rand_b(0,1);

	// maybe generate F matrix
	if(n == m && n >= 1)
	{
		if(rand_is_special_matrix(mt) < params.F_matrix_prob)
			return MatrixPtr(new FMatrix(n, rand_b(mt)));
	}

	// maybe generate OneB matrix
	if(n == 0 && m == 1)
	{
		if(rand_is_special_matrix(mt) < params.OneB_matrix_prob)
			return MatrixPtr(new OneBMatrix(rand_b(mt)));
	}

	// maybe generate Terminator matrix
	if(n == 1 && m == 0)
	{
		if(rand_is_special_matrix(mt) < params.Terminator_matrix_prob)
			return MatrixPtr(new TerminatorMatrix());
	}

	// else generate dynamic matrix
	MatrixPtr p_m(new DynamicMatrix(n,m));
	auto& matrix = *p_m;

	unsigned long long i_max_row = 1;
	unsigned long long i_max_col = 1;
	i_max_col = i_max_col << n;
	i_max_row = i_max_row << m;

	std::uniform_real_distribution<double> rand_val(0, i_max_row);

	for(std::size_t i_col = 0; i_col < i_max_col; i_col++)
	{
		std::vector<double> prob_vec(i_max_row);
		std::generate(prob_vec.begin(), prob_vec.end(), [&rand_val,&mt]() { return rand_val(mt); });
		const double total = std::accumulate(prob_vec.begin(), prob_vec.end(), 0.0);
		for (double& value: prob_vec) 
			value /= total;

		for(std::size_t i_row = 0; i_row < i_max_row; i_row++)
			matrix.set(BitVec(i_row), BitVec(i_col), prob_vec[i_row]);	
	}

	return p_m;
}
