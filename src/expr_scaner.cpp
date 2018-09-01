/*
    File:    expr_scaner.cpp
    Created: 25 August 2018 at 10:39 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cstdlib>
#include "../include/expr_scaner.h"
#include "../include/sets_for_classes.h"
#include "../include/idx_to_string.h"
#include "../include/belongs.h"
#include "../include/aux_expr_lexem.h"

namespace escaner{
    Expr_token Expr_scaner::current_lexeme()
    {
        Expr_token eti;

        aetic_        = (aeti_ = aux_scaner_->current_lexeme()).lexeme_.code_;
        lexeme_pos_   = aeti_.range_;
        lexeme_begin_ = aux_scaner_->lexeme_begin_ptr();
        switch(aetic_){
            case Aux_expr_lexem_code::Begin_char_class_complement:
                aux_scaner_->back();
                eti.lexeme_.code_                 = Expr_lexem_code::Class_complement;
                eti.lexeme_.index_of_set_of_char_ = get_set_complement();
                break;
            case Aux_expr_lexem_code::End_char_class_complement:
                eti.lexeme_.code_                 = Expr_lexem_code::UnknownLexem;
                eti.range_                        = lexeme_pos_;
            default:
                eti.lexeme_                       = convert_lexeme(aeti_);
                eti.range_                        = lexeme_pos_;
        }
        return eti;
    }

    template<typename T>
    constexpr bool is_in_segment(T value, T lower, T upper)
    {
        return (lower <= value) && (value <= upper);
    }

    static constexpr size_t first_code_of_char_class =
        static_cast<size_t>(Aux_expr_lexem_code::Class_Latin);

    static inline size_t char_class_to_array_index(Aux_expr_lexem_code e)
    {
        return static_cast<uint64_t>(e) - first_code_of_char_class;
    }

    static inline std::set<char32_t> char_class_set_by_lexeme(Aux_expr_lexem_code e)
    {
        return sets_for_char_classes[char_class_to_array_index(e)];
    }

    static const std::set<char32_t> single_quote = {U'\''};
    static const std::set<char32_t> double_quote = {U'\"'};

    Expr_lexem_info Expr_scaner::convert_lexeme(const Aux_token& aeti)
    {
        Expr_lexem_info     eli;
        Aux_expr_lexem_info aeli  = aeti.lexeme_;
        Aux_expr_lexem_code aelic = aeli.code_;

        if(is_in_segment(aelic, Aux_expr_lexem_code::M_Class_Latin,
                                Aux_expr_lexem_code::M_Class_nsq))
        {
            int y = static_cast<int>(aelic) -
                    static_cast<int>(Aux_expr_lexem_code::M_Class_Latin) +
                    static_cast<int>(Aux_expr_lexem_code::Class_Latin);
            aelic = static_cast<Aux_expr_lexem_code>(y);
        }

        switch(aelic){
            case Aux_expr_lexem_code::Character:
                eli.c_                    = aeli.c_;
                eli.code_                 = Expr_lexem_code::Character;
                break;
            case Aux_expr_lexem_code::Action:
                eli.action_name_index_    = aeli.action_name_index_;
                eli.code_                 = Expr_lexem_code::Action;
                break;
            case Aux_expr_lexem_code::Class_Latin ... Aux_expr_lexem_code::Class_xdigits:
                eli.index_of_set_of_char_ = set_trie_->insertSet(char_class_set_by_lexeme(aelic));
                eli.code_                 = Expr_lexem_code::Character_class;
                break;
            case Aux_expr_lexem_code::Class_ndq:
                eli.index_of_set_of_char_ = set_trie_->insertSet(double_quote);
                eli.code_                 = Expr_lexem_code::Class_complement;
                break;
            case Aux_expr_lexem_code::Class_nsq:
                eli.index_of_set_of_char_ = set_trie_->insertSet(single_quote);
                eli.code_                 = Expr_lexem_code::Class_complement;
                break;
            case Aux_expr_lexem_code::Regexp_name:
                eli.regexp_name_index_    = aeli.regexp_name_index_;
                eli.code_                 = Expr_lexem_code::Regexp_name;
                check_regexp_name(aeli.regexp_name_index_);
                break;
            default:
                eli.code_                 = static_cast<Expr_lexem_code>(aelic);
        }

        return eli;
    }

    static const char* undefined_regexp_name = "Error at line %zu: regexp name %s is "
                                               "undefined.\n";

    void Expr_scaner::check_regexp_name(size_t idx)
    {
        auto& idsc = scope_->idsc_;
        auto  it   = idsc.find(idx);
        if((it == idsc.end()) ||
           !belongs(static_cast<uint64_t>(Id_kind::Regexp_name), (it->second).kind_))
        {
            auto s = idx_to_string(et_.ids_trie_, idx);
            printf(undefined_regexp_name, lexeme_pos_.begin_pos_.line_no_, s.c_str());
            et_.ec_->increment_number_of_errors();
            return;
        }
    }

    size_t Expr_scaner::get_set_complement()
    {
        set_idx_ = 0;
        state_   = State::Begin_class_complement;

        curr_set_.clear();

        while((aetic_ = (aeti_ = aux_scaner_->current_lexeme()).lexeme_.code_) !=
              Aux_expr_lexem_code::Nothing)
        {
        }

//         while((aelic = (aeli = aux_scaner-> current_lexem()).code) !=
//             Aux_expr_lexem_code::Nothing)
//         {
//             (this->*procs[static_cast<size_t>(state)])();
//             if(State::End_class_complement == state){
//                 break;
//             }
//         }
        return set_idx_;
    }

    Expr_scaner::State_proc Expr_scaner::procs_[] = {
        &Expr_scaner::begin_class_complement_proc,
        &Expr_scaner::first_char_proc,
        &Expr_scaner::body_chars_proc,
        &Expr_scaner::end_class_complement_proc
    };

    void Expr_scaner::begin_class_complement_proc()
    {
        state_ = State::First_char;
    }

    static constexpr uint64_t classes_of_chars_without_complement =
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_Latin))   |
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_Letter))  |
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_Russian)) |
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_bdigits)) |
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_digits))  |
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_latin))   |
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_letter))  |
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_odigits)) |
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_russian)) |
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_xdigits));

    static constexpr uint64_t classes_of_chars_with_complement =
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_ndq)) |
        (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_nsq));

    static inline uint64_t belongs(Aux_expr_lexem_code e, uint64_t s)
    {
        return ::belongs(static_cast<uint64_t>(e), s);
    }

    void Expr_scaner::first_char_proc()
    {
        state_ = State::Body_chars;
        if(Aux_expr_lexem_code::Character == aetic_){
            curr_set_.insert(aeti_.lexeme_.c_);
        }else if(belongs(aetic_, classes_of_chars_without_complement)){
            const auto& s = sets_for_char_classes[char_class_to_array_index(aetic_)];
            curr_set_.insert(s.begin(), s.end());
        }else if(belongs(aetic_, classes_of_chars_with_complement)){
    //         printf(not_admissible_nsq_ndq, aux_scaner->lexem_begin_line_number());
    //         et_.ec->increment_number_of_errors();
    //     }else{
    //         printf(not_admissible_lexeme, aux_scaner->lexem_begin_line_number());
    //         et_.ec->increment_number_of_errors();
        }
    }

    void Expr_scaner::body_chars_proc()
    {
    //     state = State::Body_chars;
    //     if(Aux_expr_lexem_code::Character == aelic){
    //         curr_set.insert(aeli.c);
    //     }else if(belongs(aelic, classes_of_chars_without_complement)){
    //         const auto& s = sets_for_char_classes[char_class_to_array_index(aelic)];
    //         curr_set.insert(s.begin(), s.end());
    //     }else if(belongs(aelic, classes_of_chars_with_complement)){
    //         printf(not_admissible_nsq_ndq, aux_scaner->lexem_begin_line_number());
    //         et_.ec->increment_number_of_errors();
    //     }else if(Aux_expr_lexem_code::End_char_class_complement == aelic){
    //         set_idx = compl_set_trie->insertSet(curr_set);
    //         state = State::End_class_complement;
    //     }else{
    //         printf(not_admissible_lexeme, aux_scaner->lexem_begin_line_number());
    //         et_.ec->increment_number_of_errors();
    //     }
    }

    void Expr_scaner::end_class_complement_proc()
    {
    }

    // size_t Expr_scaner::lexem_begin_line_number() const
    // {
    //     return lexem_begin_line;
    // }

};

// #include <cstdio>
//
// static const char* not_admissible_nsq_ndq =
//     "Error at line %zu: character classes [:ndq:] and [:nsq:] are not admissible "
//     "in the character class complement.\n";
//
// static const char* not_admissible_lexeme =
//     "Error at line %zu: expected a character or character class, with the "
//     "exception of [:nsq:] and [:ndq:].\n";
//
// void Expr_scaner::back()
// {
//     loc->pcurrent_char = lexem_begin;
//     loc->current_line  = lexem_begin_line;
// }