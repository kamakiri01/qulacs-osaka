
#include <gtest/gtest.h>

#include <cppsim_experimental/circuit.hpp>
#include <cppsim_experimental/observable.hpp>
#include <cppsim_experimental/state.hpp>
#include <cppsim_experimental/type.hpp>
#include <cppsim_experimental/utility.hpp>
#include <csim/constant.hpp>
#include <fstream>

#include "../util/util.hpp"

TEST(ObservableTest, CheckExpectationValue) {
    const UINT n = 4;
    const UINT dim = 1ULL << n;
    const double eps = 1e-14;
    double coef;
    CPPCTYPE res;
    CPPCTYPE test_res;
    Random random;

    Eigen::MatrixXcd X(2, 2);
    X << 0, 1, 1, 0;

    StateVector state(n);
    state.set_computational_basis(0);
    Eigen::VectorXcd test_state = Eigen::VectorXcd::Zero(dim);
    test_state(0) = 1.;

    coef = random.uniform();
    Observable observable;
    observable.add_term(coef, "X 0");
    Eigen::MatrixXcd test_observable = Eigen::MatrixXcd::Zero(dim, dim);
    test_observable += coef * get_expanded_eigen_matrix_with_identity(0, X, n);

    res = observable.get_expectation_value(&state);
    test_res = (test_state.adjoint() * test_observable * test_state);
    ASSERT_NEAR(test_res.real(), res.real(), eps);
    ASSERT_NEAR(res.imag(), 0, eps);
    ASSERT_NEAR(test_res.imag(), 0, eps);

    state.set_Haar_random_state();
    for (ITYPE i = 0; i < dim; ++i) test_state[i] = state.data_cpp()[i];
    res = observable.get_expectation_value(&state);
    test_res = (test_state.adjoint() * test_observable * test_state);
    ASSERT_NEAR(test_res.real(), res.real(), eps);
    ASSERT_NEAR(res.imag(), 0, eps);
    ASSERT_NEAR(test_res.imag(), 0, eps);

    for (UINT repeat = 0; repeat < 10; ++repeat) {
        Observable rand_observable;
        Eigen::MatrixXcd test_rand_observable =
            Eigen::MatrixXcd::Zero(dim, dim);

        UINT term_count = random.int32() % 10 + 1;
        for (UINT term = 0; term < term_count; ++term) {
            std::vector<UINT> paulis(n, 0);
            Eigen::MatrixXcd test_rand_observable_term =
                Eigen::MatrixXcd::Identity(dim, dim);
            coef = random.uniform();
            for (UINT i = 0; i < paulis.size(); ++i) {
                paulis[i] = random.int32() % 4;

                test_rand_observable_term *=
                    get_expanded_eigen_matrix_with_identity(
                        i, get_eigen_matrix_single_Pauli(paulis[i]), n);
            }
            test_rand_observable += coef * test_rand_observable_term;

            std::string str = "";
            for (UINT ind = 0; ind < paulis.size(); ind++) {
                UINT val = paulis[ind];
                if (val != 0) {
                    if (val == 1)
                        str += " X";
                    else if (val == 2)
                        str += " Y";
                    else if (val == 3)
                        str += " Z";
                    str += " " + std::to_string(ind);
                }
            }
            rand_observable.add_term(coef, str.c_str());
        }

        state.set_Haar_random_state();
        for (ITYPE i = 0; i < dim; ++i) test_state[i] = state.data_cpp()[i];

        res = rand_observable.get_expectation_value(&state);
        test_res = test_state.adjoint() * test_rand_observable * test_state;
        ASSERT_NEAR(test_res.real(), res.real(), eps);
        ASSERT_NEAR(res.imag(), 0, eps);
        ASSERT_NEAR(test_res.imag(), 0, eps);
    }
}

TEST(ObservableTest, to_stringTest) {
    std::string expected =
        "(1-1j) [X 1 Y 2 Z 3 ] +\n"
        "(0.2+0.2j) [X 4 Y 5 Z 6 ]";
    Observable observable;
    observable.add_term(1.0 - 1.0i, "X 1 Y 2 Z 3");
    observable.add_term(0.2 + 0.2i, "X 4 Y 5 Z 6");
    EXPECT_EQ(expected, observable.to_string());
}

TEST(ObservableTest, to_string_SignOfCoefTest) {
#ifndef _MSC_VER
    std::string expected =
        "(0-0j) [X 0 ] +\n"
        "(0+0j) [Y 0 ]";
#else
    std::string expected =
        "(0+0j) [X 0 ] +\n"
        "(0+0j) [Y 0 ]";
#endif
    Observable observable;
    observable.add_term(0.0 - 0.0i, "X 0");
    observable.add_term(0.0 + 0.0i, "Y 0");
    EXPECT_EQ(expected, observable.to_string());
}

TEST(ObservableTest, calc_coefTest) {
    Observable X, Y, Z;
    X.add_term(1.0, "X 0");
    Y.add_term(1.0, "Y 0");
    Z.add_term(1.0, "Z 0");

    // XY = iZ
    Observable XY = X * Y;
    EXPECT_EQ(1i, XY.get_term(0).first);
    EXPECT_EQ(PAULI_ID_Z, XY.get_term(0).second.get_pauli_id_list().at(0));
    // XZ = -iY
    Observable XZ = X * Z;
    EXPECT_EQ(-1i, XZ.get_term(0).first);
    EXPECT_EQ(PAULI_ID_Y, XZ.get_term(0).second.get_pauli_id_list().at(0));
    // YX = -iZ
    Observable YX = Y * X;
    EXPECT_EQ(-1i, YX.get_term(0).first);
    EXPECT_EQ(PAULI_ID_Z, YX.get_term(0).second.get_pauli_id_list().at(0));
    // YZ = iX
    Observable YZ = Y * Z;
    EXPECT_EQ(1i, YZ.get_term(0).first);
    EXPECT_EQ(PAULI_ID_X, YZ.get_term(0).second.get_pauli_id_list().at(0));
    // ZX = iY
    Observable ZX = Z * X;
    EXPECT_EQ(1i, ZX.get_term(0).first);
    EXPECT_EQ(PAULI_ID_Y, ZX.get_term(0).second.get_pauli_id_list().at(0));
    // ZY = -iX
    Observable ZY = Z * Y;
    EXPECT_EQ(-1i, ZY.get_term(0).first);
    EXPECT_EQ(PAULI_ID_X, ZY.get_term(0).second.get_pauli_id_list().at(0));
}

TEST(ObservableTest, remove_term_Test) {
    Observable ob;
    ob.add_term(1.0, "X 0");
    ob.add_term(1.0, "Y 1");
    ob.add_term(1.0, "Z 2");
    ob.add_term(1.0, "X 3");

    auto dict1 = ob.get_dict();
    EXPECT_EQ(4, dict1.size());
    EXPECT_EQ(0, dict1["X 0 "]);
    EXPECT_EQ(1, dict1["Y 1 "]);
    EXPECT_EQ(2, dict1["Z 2 "]);
    EXPECT_EQ(3, dict1["X 3 "]);

    ob.remove_term(1);
    auto dict2 = ob.get_dict();
    EXPECT_EQ(3, dict2.size());
    EXPECT_EQ(0, dict2["X 0 "]);
    EXPECT_EQ(1, dict2["Z 2 "]);
    EXPECT_EQ(2, dict2["X 3 "]);
}

TEST(ObservableTest, term_dict_SizeTest) {
    Observable x_term, y_term, xy_term;
    Observable ob;
    Observable res, res1;
    x_term.add_term(1.0, "X 0");
    y_term.add_term(1.0, "Y 0");

    xy_term = x_term + y_term;
    EXPECT_EQ(2, xy_term.get_dict().size());

    ob.add_term(1.0, "I 0");
    ob *= xy_term;
    EXPECT_EQ(2, ob.get_dict().size());

    res += ob;
    EXPECT_EQ(2, res.get_dict().size());

    res.remove_term(0);
    EXPECT_EQ(1, res.get_dict().size());

    res1 += res;
    EXPECT_EQ(1, res1.get_dict().size());
}

/*
TEST(ObservableTest, CheckParsedObservableFromOpenFermionFile) {
    auto func = [](const std::string path,
                    const QuantumStateBase* state) -> CPPCTYPE {
        std::ifstream ifs;
        ifs.open(path);
        if (!ifs) {
            std::cerr << "ERROR: Cannot open file" << std::endl;
            return -1.;
        }

        CPPCTYPE energy = 0;

        std::string str;
        while (getline(ifs, str)) {
            // std::cout << state->get_norm() << std::endl;

            std::vector<std::string> elems;
            elems = split(str, "()j[]+");

            chfmt(elems[3]);

            CPPCTYPE coef(std::stod(elems[0]), std::stod(elems[1]));
            // std::cout << elems[3].c_str() << std::endl;

            PauliOperator mpt(elems[3].c_str(), coef.real());

            // std::cout << mpt.get_coef() << " ";
            // std::cout << elems[3].c_str() << std::endl;
            energy += mpt.get_expectation_value(state);
            // mpt.get_expectation_value(state);
        }
        if (!ifs.eof()) {
            std::cerr << "ERROR: Invalid format" << std::endl;
            return -1.;
        }
        ifs.close();
        return energy;
    };

    const double eps = 1e-14;
    const char* filename = "../test/cppsim_experimental/H2.txt";

    CPPCTYPE res, test_res;

    Observable* observable;
    observable = observable::create_observable_from_openfermion_file(filename);
    ASSERT_NE(observable, (Observable*)NULL);
    UINT qubit_count = observable->get_qubit_count();

    StateVector state(qubit_count);
    state.set_computational_basis(0);

    res = observable->get_expectation_value(&state);
    test_res = func(filename, &state);

    ASSERT_EQ(test_res, res);

    state.set_Haar_random_state();

    res = observable->get_expectation_value(&state);
    test_res = func(filename, &state);

    ASSERT_NEAR(test_res.real(), res.real(), eps);
    ASSERT_NEAR(test_res.imag(), 0, eps);
    ASSERT_NEAR(res.imag(), 0, eps);
}

TEST(ObservableTest, CheckParsedObservableFromOpenFermionText) {
    auto func = [](const std::string str,
                    const QuantumStateBase* state) -> CPPCTYPE {
        CPPCTYPE energy = 0;

        std::vector<std::string> lines = split(str, "\n");

        for (std::string line : lines) {
            // std::cout << state->get_norm() << std::endl;

            std::vector<std::string> elems;
            elems = split(line, "()j[]+");

            chfmt(elems[3]);

            CPPCTYPE coef(std::stod(elems[0]), std::stod(elems[1]));
            // std::cout << elems[3].c_str() << std::endl;

            PauliOperator mpt(elems[3].c_str(), coef.real());

            // std::cout << mpt.get_coef() << " ";
            // std::cout << elems[3].c_str() << std::endl;
            energy += mpt.get_expectation_value(state);
            // mpt.get_expectation_value(state);
        }
        return energy;
    };

    const double eps = 1e-14;
    const std::string text =
        "(-0.8126100000000005+0j) [] +\n"
        "(0.04532175+0j) [X0 Z1 X2] +\n"
        "(0.04532175+0j) [X0 Z1 X2 Z3] +\n"
        "(0.04532175+0j) [Y0 Z1 Y2] +\n"
        "(0.04532175+0j) [Y0 Z1 Y2 Z3] +\n"
        "(0.17120100000000002+0j) [Z0] +\n"
        "(0.17120100000000002+0j) [Z0 Z1] +\n"
        "(0.165868+0j) [Z0 Z1 Z2] +\n"
        "(0.165868+0j) [Z0 Z1 Z2 Z3] +\n"
        "(0.12054625+0j) [Z0 Z2] +\n"
        "(0.12054625+0j) [Z0 Z2 Z3] +\n"
        "(0.16862325+0j) [Z1] +\n"
        "(-0.22279649999999998+0j) [Z1 Z2 Z3] +\n"
        "(0.17434925+0j) [Z1 Z3] +\n"
        "(-0.22279649999999998+0j) [Z2]";

    CPPCTYPE res, test_res;

    Observable* observable;
    observable = observable::create_observable_from_openfermion_text(text);
    ASSERT_NE(observable, (Observable*)NULL);
    UINT qubit_count = observable->get_qubit_count();

    StateVector state(qubit_count);
    state.set_computational_basis(0);

    res = observable->get_expectation_value(&state);
    test_res = func(text, &state);

    ASSERT_EQ(test_res, res);

    state.set_Haar_random_state();

    res = observable->get_expectation_value(&state);
    test_res = func(text, &state);

    ASSERT_NEAR(test_res.real(), res.real(), eps);
    ASSERT_NEAR(test_res.imag(), 0, eps);
    ASSERT_NEAR(res.imag(), 0, eps);
}

TEST(ObservableTest, CheckSplitObservable) {
    auto func = [](const std::string path,
                    const QuantumStateBase* state) -> CPPCTYPE {
        std::ifstream ifs;
        CPPCTYPE coef;
        ifs.open(path);
        if (!ifs) {
            std::cerr << "ERROR: Cannot open file" << std::endl;
            return -1.;
        }

        CPPCTYPE energy = 0;

        std::string str;
        while (getline(ifs, str)) {
            // std::cout << state->get_norm() << std::endl;

            std::vector<std::string> elems;
            elems = split(str, "()j[]+");

            chfmt(elems[3]);

            CPPCTYPE coef(std::stod(elems[0]), std::stod(elems[1]));
            // std::cout << elems[3].c_str() << std::endl;

            PauliOperator mpt(elems[3].c_str(), coef.real());

            // std::cout << mpt.get_coef() << " ";
            // std::cout << elems[3].c_str() << std::endl;
            energy += mpt.get_expectation_value(state);
            // mpt.get_expectation_value(state);
        }
        if (!ifs.eof()) {
            std::cerr << "ERROR: Invalid format" << std::endl;
            return -1.;
        }
        ifs.close();
        return energy;
    };

    const double eps = 1e-14;
    const char* filename = "../test/cppsim_experimental/H2.txt";

    CPPCTYPE diag_res, test_res, non_diag_res;

    std::pair<Observable*, Observable*> observables;
    observables = observable::create_split_observable(filename);
    ASSERT_NE(observables.first, (Observable*)NULL);
    ASSERT_NE(observables.second, (Observable*)NULL);

    UINT qubit_count = observables.first->get_qubit_count();
    StateVector state(qubit_count);
    state.set_computational_basis(0);

    diag_res = observables.first->get_expectation_value(&state);
    non_diag_res = observables.second->get_expectation_value(&state);
    test_res = func(filename, &state);

    ASSERT_NEAR(test_res.real(), (diag_res + non_diag_res).real(), eps);
    ASSERT_NEAR(test_res.imag(), 0, eps);
    ASSERT_NEAR(diag_res.imag(), 0, eps);
    ASSERT_NEAR(non_diag_res.imag(), 0, eps);

    state.set_Haar_random_state();

    diag_res = observables.first->get_expectation_value(&state);
    non_diag_res = observables.second->get_expectation_value(&state);
    test_res = func(filename, &state);

    ASSERT_NEAR(test_res.real(), (diag_res + non_diag_res).real(), eps);
    ASSERT_NEAR(test_res.imag(), 0, eps);
    ASSERT_NEAR(diag_res.imag(), 0, eps);
    ASSERT_NEAR(non_diag_res.imag(), 0, eps);
}
*/