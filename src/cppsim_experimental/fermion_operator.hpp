#pragma once

#include "observable.hpp"
#include "single_fermion_operator.hpp"
#include "state.hpp"
#include "type.hpp"

/**
 * \~japanese-en
 * @struct FermionOperator
 * オブザーバブルのように、SingleFermionOperatorをリストとして持ち、操作を行うクラス。
 */
class DllExport FermionOperator {
private:
    std::vector<SingleFermionOperator> _fermion_terms;
    std::vector<CPPCTYPE> _coef_list;
    std::unordered_map<std::string, ITYPE> _term_dict;

public:
    FermionOperator();

    /**
     * FermionOperator が保持する SingleFermionOperator の個数を返す
     * @return FermionOperator が保持する SingleFermionOperator の個数
     */
    UINT get_term_count() const;

    /**
     * FermionOperator の指定した添字に対応するSingleFermionOperatorを返す
     * @param[in] index
     * FermionOperator が保持するSingleFermionOperatorのリストの添字
     * @return 指定したindexにあるSingleFermionOperator
     */
    std::pair<CPPCTYPE, SingleFermionOperator> get_term(const UINT index) const;
    /**
     * SingleFermionOperatorを内部で保持するリストの末尾に追加する。
     *
     * @param[in] coef 係数
     * @param[in] fermion_operator 追加するSingleFermionOperatorのインスタンス
     */
    void add_term(const CPPCTYPE coef, SingleFermionOperator fermion_operator);

    /**
     * 係数とフェルミオン演算子の文字列の組を追加する。
     *
     * @param[in] coef
     * @param[in] action_string
     * 演算子と作用する軌道の添字の組からなる文字列。(example: "2^ 1")
     */
    void add_term(const CPPCTYPE coef, std::string action_string);

    void add_term(const std::vector<CPPCTYPE> coef_list,
        std::vector<SingleFermionOperator> fermion_terms);

    /**
     * 指定した添字に対応するフェルミオン演算子の項を削除する
     * @param[in] index
     * FermionOperator が保持するSingleFermionOperatorのリストの添字
     */
    void remove_term(UINT index);

    /**
     * 各項のSingleFermionOperatorのリストを取得する
     */
    const std::vector<SingleFermionOperator>& get_fermion_list() const;

    /**
     * 各項の係数のリストを取得する
     */
    const std::vector<CPPCTYPE>& get_coef_list() const;

    const std::unordered_map<std::string, ITYPE>& get_dict() const;

    FermionOperator* copy() const;

    FermionOperator operator+(const FermionOperator& target) const;

    FermionOperator operator+=(const FermionOperator& target);

    FermionOperator operator-(const FermionOperator& target) const;

    FermionOperator operator-=(const FermionOperator& target);

    FermionOperator operator*(const FermionOperator& target) const;

    FermionOperator operator*(const CPPCTYPE& target) const;

    FermionOperator operator*=(const FermionOperator& target);

    FermionOperator operator*=(const CPPCTYPE& target);
};
