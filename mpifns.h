//***************************************************************************
//  NARS2000 -- MP Infinity Cover Functions
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#ifndef MPIFNS
#define MPIFNS

#undef  mpz_init_set
#undef  mpz_init_set_d

#undef  mpz_set
#undef  mpz_set_ui
#undef  mpz_set_si
#undef  mpz_set_d
#undef  mpz_set_q
#undef  mpz_set_f

#undef  mpz_get_ui
#undef  mpz_get_si
#undef  mpz_get_d
#undef  mpz_get_d_2exp
#undef  mpz_get_str

#undef  mpz_add
#undef  mpz_add_ui

#undef  mpz_sub
#undef  mpz_sub_ui
#undef  mpz_ui_sub

#undef  mpz_mul
#undef  mpz_mul_si
#undef  mpz_mul_ui

#undef  mpz_addmul
#undef  mpz_addmul_ui

#undef  mpz_submul
#undef  mpz_submul_ui
#undef  mpz_mul_2exp

#undef  mpz_neg
#undef  mpz_abs

#undef  mpz_cdiv_q
#undef  mpz_cdiv_r
#undef  mpz_cdiv_qr
#undef  mpz_cdiv_q_ui
#undef  mpz_cdiv_r_ui
#undef  mpz_cdiv_qr_ui
#undef  mpz_cdiv_ui
#undef  mpz_cdiv_q_2exp
#undef  mpz_cdiv_r_2exp

#undef  mpz_fdiv_q
#undef  mpz_fdiv_r
#undef  mpz_fdiv_qr
#undef  mpz_fdiv_q_ui
#undef  mpz_fdiv_r_ui
#undef  mpz_fdiv_qr_ui
#undef  mpz_fdiv_ui
#undef  mpz_fdiv_q_2exp
#undef  mpz_fdiv_r_2exp

#undef  mpz_tdiv_q
#undef  mpz_tdiv_r
#undef  mpz_tdiv_qr
#undef  mpz_tdiv_q_ui
#undef  mpz_tdiv_r_ui
#undef  mpz_tdiv_qr_ui
#undef  mpz_tdiv_ui
#undef  mpz_tdiv_q_2exp
#undef  mpz_tdiv_r_2exp

#undef  mpz_mod
#undef  mpz_mod_ui
#undef  mpz_divexact
#undef  mpz_divexact_ui

#undef  mpz_divisible_p
#undef  mpz_divisible_ui_p
#undef  mpz_divisible_2exp_p

#undef  mpz_congruent_p
#undef  mpz_congruent_ui_p
#undef  mpz_congruent_2exp_p

#undef  mpz_powm
#undef  mpz_powm_ui

#undef  mpz_pow_ui
#undef  mpz_ui_pow_ui

#undef  mpz_root
#undef  mpz_nthroot
#undef  mpz_rootrem
#undef  mpz_sqrt
#undef  mpz_sqrtrem
#undef  mpz_perfect_power_p
#undef  mpz_perfect_square
#undef  mpz_probable_prime_p
#undef  mpz_likely_prime_p
#undef  mpz_probab_prime_p
#undef  mpz_nextprime
#undef  mpz_next_likely_prime
#undef  mpz_gcd
#undef  mpz_gcd_ui
#undef  mpz_gcdext
#undef  mpz_lcm
#undef  mpz_lcm_ui
#undef  mpz_invert
#undef  mpz_jacobi
#undef  mpz_kronecker
#undef  mpz_kronecker_si
#undef  mpz_kronecker_ui
#undef  mpz_si_kronecker
#undef  mpz_ui_kronecker
#undef  mpz_remove

#undef  mpz_cmp
#undef  mpz_cmp_d
#undef  mpz_cmp_si
#undef  mpz_cmp_ui
#undef  mpz_cmpabs
#undef  mpz_cmpabs_d
#undef  mpz_cmpabs_ui

#undef  mpz_and
#undef  mpz_ior
#undef  mpz_xor
#undef  mpz_com
#undef  mpz_popcount
#undef  mpz_hamdist
#undef  mpz_scan0
#undef  mpz_scan1
#undef  mpz_setbit
#undef  mpz_clrbit
#undef  mpz_combit
#undef  mpz_tstbit

#undef  mpz_out_str
#undef  mpz_inp_str
#undef  mpz_out_raw
#undef  mpz_inp_raw

#undef  mpz_fits_ulong_p
#undef  mpz_fits_slong_p
#undef  mpz_fits_uint_p
#undef  mpz_fits_sint_p
#undef  mpz_fits_ushort_p
#undef  mpz_fits_sshort_p

#undef  mpz_odd_p
#undef  mpz_even_p

#undef  mpz_sizeinbase
#undef  _mpz_realloc
#undef  mpz_getlimbn
#undef  mpz_size

#undef  mpq_canonicalize

#undef  mpq_set
#undef  mpq_set_z
#undef  mpq_set_ui
#undef  mpq_set_si
#undef  mpq_set_str

#undef  mpq_get_d

#undef  mpq_set_d
#undef  mpq_set_f

#undef  mpq_get_str

#undef  mpq_add
#undef  mpq_sub
#undef  mpq_mul
#undef  mpq_mul_2exp
#undef  mpq_div
#undef  mpq_div_2exp
#undef  mpq_neg
#undef  mpq_abs
#undef  mpq_inv
#undef  mpq_cmp
#undef  mpq_cmp_ui
#undef  mpq_cmp_si
#undef  mpq_equal

#undef  mpq_out_str
#undef  mpq_inp_str

#undef  mpf_set_d
#undef  mpf_set_z
#undef  mpf_set_q
#undef  mpf_set_str

#undef  mpf_init_set_d
#undef  mpf_init_set_str

#undef  mpf_get_d
#undef  mpf_get_d_2exp
#undef  mpf_get_si
#undef  mpf_get_ui
#undef  mpf_get_str

#undef  mpf_add
#undef  mpf_add_ui

#undef  mpf_sub
#undef  mpf_ui_sub
#undef  mpf_sub_ui

#undef  mpf_mul
#undef  mpf_mul_ui

#undef  mpf_div
#undef  mpf_ui_div
#undef  mpf_div_ui

#undef  mpf_sqrt
#undef  mpf_pow_ui
#undef  mpf_neg
#undef  mpf_abs
#undef  mpf_mul_2exp
#undef  mpf_div_2exp

#undef  mpf_cmp
#undef  mpf_cmp_d
#undef  mpf_cmp_ui
#undef  mpf_cmp_si
#undef  mpf_eq

#undef  mpf_out_str
#undef  mpf_inp_str

#undef  mpf_reldiff
#undef  mpf_ceil
#undef  mpf_floor
#undef  mpf_trunc
#undef  mpf_integer_p

#undef  mpf_fits_ulong_p
#undef  mpf_fits_slong_p
#undef  mpf_fits_uint_p
#undef  mpf_fits_sint_p
#undef  mpf_fits_ushort_p
#undef  mpf_fits_sshort_p


#define mpz_init_set            mpiz_init_set
#define mpz_init_set_d          mpiz_init_set_d

#define mpz_set                 mpiz_set
#define mpz_set_ui              mpiz_set_ui
#define mpz_set_si              mpiz_set_si
#define mpz_set_d               mpiz_set_d
#define mpz_set_q               mpiz_set_q
#define mpz_set_f               mpiz_set_f

#define mpz_get_ui              mpiz_get_ui
#define mpz_get_si              mpiz_get_si
#define mpz_get_d               mpiz_get_d
#define mpz_get_d_2exp          mpiz_get_d_2exp
#define mpz_get_str             mpiz_get_str

#define mpz_add                 mpiz_add
#define mpz_add_ui              mpiz_add_ui

#define mpz_sub                 mpiz_sub
#define mpz_sub_ui              mpiz_sub_ui
#define mpz_ui_sub              mpiz_ui_sub

#define mpz_mul                 mpiz_mul
#define mpz_mul_si              mpiz_mul_si
#define mpz_mul_ui              mpiz_mul_ui

#define mpz_addmul              mpiz_addmul
#define mpz_addmul_ui           mpiz_addmul_ui

#define mpz_submul              mpiz_submul
#define mpz_submul_ui           mpiz_submul_ui
#define mpz_mul_2exp            mpiz_mul_2exp

#define mpz_neg                 mpiz_neg
#define mpz_abs                 mpiz_abs

#define mpz_cdiv_q              mpiz_cdiv_q
#define mpz_cdiv_r              mpiz_cdiv_r
#define mpz_cdiv_qr             mpiz_cdiv_qr
#define mpz_cdiv_q_ui           mpiz_cdiv_q_ui
#define mpz_cdiv_r_ui           mpiz_cdiv_r_ui
#define mpz_cdiv_qr_ui          mpiz_cdiv_qr_ui
#define mpz_cdiv_ui             mpiz_cdiv_ui
#define mpz_cdiv_q_2exp         mpiz_cdiv_q_2exp
#define mpz_cdiv_r_2exp         mpiz_cdiv_r_2exp

#define mpz_fdiv_q              mpiz_fdiv_q
#define mpz_fdiv_r              mpiz_fdiv_r
#define mpz_fdiv_qr             mpiz_fdiv_qr
#define mpz_fdiv_q_ui           mpiz_fdiv_q_ui
#define mpz_fdiv_r_ui           mpiz_fdiv_r_ui
#define mpz_fdiv_qr_ui          mpiz_fdiv_qr_ui
#define mpz_fdiv_ui             mpiz_fdiv_ui
#define mpz_fdiv_q_2exp         mpiz_fdiv_q_2exp
#define mpz_fdiv_r_2exp         mpiz_fdiv_r_2exp

#define mpz_tdiv_q              mpiz_tdiv_q
#define mpz_tdiv_r              mpiz_tdiv_r
#define mpz_tdiv_qr             mpiz_tdiv_qr
#define mpz_tdiv_q_ui           mpiz_tdiv_q_ui
#define mpz_tdiv_r_ui           mpiz_tdiv_r_ui
#define mpz_tdiv_qr_ui          mpiz_tdiv_qr_ui
#define mpz_tdiv_ui             mpiz_tdiv_ui
#define mpz_tdiv_q_2exp         mpiz_tdiv_q_2exp
#define mpz_tdiv_r_2exp         mpiz_tdiv_r_2exp

#define mpz_mod                 mpiz_mod
#define mpz_mod_ui              mpiz_mod_ui
#define mpz_divexact            mpiz_divexact
#define mpz_divexact_ui         mpiz_divexact_ui

#define mpz_divisible_p         mpiz_divisible_p
#define mpz_divisible_ui_p      mpiz_divisible_ui_p
#define mpz_divisible_2exp_p    mpiz_divisible_2exp_p

#define mpz_congruent_p         mpiz_congruent_p
#define mpz_congruent_ui_p      mpiz_congruent_ui_p
#define mpz_congruent_2exp_p    mpiz_congruent_2exp_p

#define mpz_powm                mpiz_powm
#define mpz_powm_ui             mpiz_powm_ui

#define mpz_pow_ui              mpiz_pow_ui
#define mpz_ui_pow_ui           mpiz_ui_pow_ui

#define mpz_root                mpiz_root
#define mpz_nthroot             mpiz_nthroot
#define mpz_rootrem             mpiz_rootrem
#define mpz_sqrt                mpiz_sqrt
#define mpz_sqrtrem             mpiz_sqrtrem
#define mpz_perfect_power_p     mpiz_perfect_power_p
#define mpz_perfect_square      mpiz_perfect_square
#define mpz_probable_prime_p    mpiz_probable_prime_p
#define mpz_likely_prime_p      mpiz_likely_prime_p
#define mpz_probab_prime_p      mpiz_probab_prime_p
#define mpz_nextprime           mpiz_nextprime
#define mpz_next_likely_prime   mpiz_next_likely_prime
#define mpz_gcd                 mpiz_gcd
#define mpz_gcd_ui              mpiz_gcd_ui
#define mpz_gcdext              mpiz_gcdext
#define mpz_lcm                 mpiz_lcm
#define mpz_lcm_ui              mpiz_lcm_ui
#define mpz_invert              mpiz_invert
#define mpz_jacobi              mpiz_jacobi
#define mpz_kronecker           mpiz_kronecker
#define mpz_kronecker_si        mpiz_kronecker_si
#define mpz_kronecker_ui        mpiz_kronecker_ui
#define mpz_si_kronecker        mpiz_si_kronecker
#define mpz_ui_kronecker        mpiz_ui_kronecker
#define mpz_remove              mpiz_remove

#define mpz_cmp                 mpiz_cmp
#define mpz_cmp_d               mpiz_cmp_d
#define mpz_cmp_si              mpiz_cmp_si
#define mpz_cmp_ui              mpiz_cmp_ui
#define mpz_cmpabs              mpiz_cmpabs
#define mpz_cmpabs_d            mpiz_cmpabs_d
#define mpz_cmpabs_ui           mpiz_cmpabs_ui

#define mpz_and                 mpiz_and
#define mpz_ior                 mpiz_ior
#define mpz_xor                 mpiz_xor
#define mpz_com                 mpiz_com
#define mpz_popcount            mpiz_popcount
#define mpz_hamdist             mpiz_hamdist
#define mpz_scan0               mpiz_scan0
#define mpz_scan1               mpiz_scan1
#define mpz_setbit              mpiz_setbit
#define mpz_clrbit              mpiz_clrbit
#define mpz_combit              mpiz_combit
#define mpz_tstbit              mpiz_tstbit

#define mpz_out_str             mpiz_out_str
#define mpz_inp_str             mpiz_inp_str
#define mpz_out_raw             mpiz_out_raw
#define mpz_inp_raw             mpiz_inp_raw

#define mpz_fits_ulong_p        mpiz_fits_ulong_p
#define mpz_fits_slong_p        mpiz_fits_slong_p
#define mpz_fits_uint_p         mpiz_fits_uint_p
#define mpz_fits_sint_p         mpiz_fits_sint_p
#define mpz_fits_ushort_p       mpiz_fits_ushort_p
#define mpz_fits_sshort_p       mpiz_fits_sshort_p

#define mpz_odd_p               mpiz_odd_p
#define mpz_even_p              mpiz_even_p

#define mpz_sizeinbase          mpiz_sizeinbase
#define _mpz_realloc            _mpiz_realloc
#define mpz_getlimbn            mpiz_getlimbn
#define mpz_size                mpiz_size

#define mpq_canonicalize        mpiq_canonicalize

#define mpq_set                 mpiq_set
#define mpq_set_z               mpiq_set_z
#define mpq_set_ui              mpiq_set_ui
#define mpq_set_si              mpiq_set_si
#define mpq_set_str             mpiq_set_str

#define mpq_get_d               mpiq_get_d

#define mpq_set_d               mpiq_set_d
#define mpq_set_f               mpiq_set_f

#define mpq_get_str             mpiq_get_str

#define mpq_add                 mpiq_add
#define mpq_sub                 mpiq_sub
#define mpq_mul                 mpiq_mul
#define mpq_mul_2exp            mpiq_mul_2exp
#define mpq_div                 mpiq_div
#define mpq_div_2exp            mpiq_div_2exp
#define mpq_neg                 mpiq_neg
#define mpq_abs                 mpiq_abs
#define mpq_inv                 mpiq_inv
#define mpq_cmp                 mpiq_cmp
#define mpq_cmp_ui              mpiq_cmp_ui
#define mpq_cmp_si              mpiq_cmp_si
#define mpq_equal               mpiq_equal

#define mpq_out_str             mpiq_out_str
#define mpq_inp_str             mpiq_inp_str

#define mpf_set_d               mpif_set_d
#define mpf_set_z               mpif_set_z
#define mpf_set_q               mpif_set_q
#define mpf_set_str             mpif_set_str

#define mpf_init_set_d          mpif_init_set_d
#define mpf_init_set_str        mpif_init_set_str

#define mpf_get_d               mpif_get_d
#define mpf_get_d_2exp          mpif_get_d_2exp
#define mpf_get_si              mpif_get_si
#define mpf_get_ui              mpif_get_ui
#define mpf_get_str             mpif_get_str

#define mpf_add                 mpif_add
#define mpf_add_ui              mpif_add_ui

#define mpf_sub                 mpif_sub
#define mpf_ui_sub              mpif_ui_sub
#define mpf_sub_ui              mpif_sub_ui

#define mpf_mul                 mpif_mul
#define mpf_mul_ui              mpif_mul_ui

#define mpf_div                 mpif_div
#define mpf_ui_div              mpif_ui_div
#define mpf_div_ui              mpif_div_ui

#define mpf_sqrt                mpif_sqrt
#define mpf_pow_ui              mpif_pow_ui
#define mpf_neg                 mpif_neg
#define mpf_abs                 mpif_abs
#define mpf_mul_2exp            mpif_mul_2exp
#define mpf_div_2exp            mpif_div_2exp

#define mpf_cmp                 mpif_cmp
#define mpf_cmp_d               mpif_cmp_d
#define mpf_cmp_ui              mpif_cmp_ui
#define mpf_cmp_si              mpif_cmp_si
#define mpf_eq                  mpif_eq

#define mpf_out_str             mpif_out_str
#define mpf_inp_str             mpif_inp_str

#define mpf_reldiff             mpif_reldiff
#define mpf_ceil                mpif_ceil
#define mpf_floor               mpif_floor
#define mpf_trunc               mpif_trunc
#define mpf_integer_p           mpif_integer_p

#define mpf_fits_ulong_p        mpif_fits_ulong_p
#define mpf_fits_slong_p        mpif_fits_slong_p
#define mpf_fits_uint_p         mpif_fits_uint_p
#define mpf_fits_sint_p         mpif_fits_sint_p
#define mpf_fits_ushort_p       mpif_fits_ushort_p
#define mpf_fits_sshort_p       mpif_fits_sshort_p

#endif


//***************************************************************************
//  End of File: mpifns.h
//***************************************************************************
