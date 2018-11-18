/*
     File:    expr-scaner-test.cpp
     Created: 25 August 2018 at 15:59 Moscow time
     Author:  Гаврилов Владимир Сергеевич
     E-mails: vladimir.s.gavrilov@gmail.com
              gavrilov.vladimir.s@mail.ru
              gavvs1977@yandex.ru
*/

#include <string>
#include <cstdio>
#include <memory>
#include "../include/get_processed_text.h"
#include "../include/location.h"
#include "../include/errors_and_tries.h"
#include "../include/error_count.h"
#include "../include/char_trie.h"
#include "../include/scope.h"
#include "../include/expr_scaner.h"
#include "../include/trie_for_set.h"
#include "../include/char_conv.h"

enum Myauka_exit_codes{
    Success, No_args, File_processing_error, Syntax_error
};

static const char* usage_str = "Usage: %s file\n";

void test_expr_scaner(const std::shared_ptr<escaner::Expr_scaner>& expr_scaner)
{
    escaner::Expr_token      eli;
    escaner::Expr_lexem_code elic;
    do{
        eli    = expr_scaner->current_lexeme();
        elic   = eli.lexeme_.code_;
        auto s = expr_scaner->token_to_string(eli);
        puts(s.c_str());
    }while(elic != escaner::Expr_lexem_code::Nothing);
}

// static const char32_t* write_act_name         = U"write";
// static const char32_t* write_act_body         = U"buffer += ch;";
//
// static const char32_t* add_dec_digit_act_name = U"add_dec_digit_to_char_code";
// static const char32_t* add_dec_digit_act_body = U"char_code = char_code * 10 + digit2int(ch);";
//
// static const char32_t* add_hex_digit_act_name = U"add_hex_digit_to_char_code";
// static const char32_t* add_hex_digit_act_body = U"char_code = char_code << 4 + digit2int(ch);";
//
// static const char32_t* add_bin_digit_act_name = U"add_bin_digit_to_char_code";
// static const char32_t* add_bin_digit_act_body = U"char_code = char_code << 1 + digit2int(ch);";
//
// static const char32_t* add_oct_digit_act_name = U"add_oct_digit_to_char_code";
// static const char32_t* add_oct_digit_act_body = U"char_code = char_code << 3 + digit2int(ch);";
//
// static const char32_t* write_by_code_act_name = U"write_by_code";
// static const char32_t* write_by_code_act_body = U"buffer += char_code;";
//
// static const char32_t* add_dec_digit_act_name1 = U"add_dec_digit";
// static const char32_t* add_dec_digit_act_body1 = U"token.value = token.value * 10 + digit2int(ch);";
//
// static const char32_t* add_hex_digit_act_name1 = U"add_hex_digit";
// static const char32_t* add_hex_digit_act_body1 = U"token.value = token.value << 4 + digit2int(ch);";
//
// static const char32_t* add_bin_digit_act_name1 = U"add_bin_digit";
// static const char32_t* add_bin_digit_act_body1 = U"token.value = token.value << 1 + digit2int(ch);";
//
// static const char32_t* add_oct_digit_act_name1 = U"add_oct_digit";
// static const char32_t* add_oct_digit_act_body1 = U"token.value = token.value << 3 + digit2int(ch);";
//
// void add_action(Errors_and_tries&       etr,
//                 std::shared_ptr<Scope>& scope,
//                 const std::u32string&   name,
//                 const std::u32string&   body)
// {
//     Id_attributes iattr;
//     iattr.kind_             = 1u << static_cast<uint8_t>(Id_kind::Action_name);
//     size_t idx              = etr.ids_trie_ -> insert(name);
//     size_t body_idx         = etr.strs_trie_-> insert(body);
//     iattr.act_string_       = body_idx;
//     scope->idsc_[idx]       = iattr;
//
//     Str_attributes sattr;
//     sattr.kind_             = 1u << static_cast<uint16_t>(Str_kind::Action_definition);
//     sattr.code_             = 0;
//     scope->strsc_[body_idx] = sattr;
//
//     auto name_in_utf8 = u32string_to_utf8(name);
//     printf("Index of action with name %s is %zu.\n",
//            name_in_utf8.c_str(), idx);
// }

int main(int argc, char* argv[])
{
    if(1 == argc){
        printf(usage_str, argv[0]);
        return No_args;
    }

    auto              text   = get_processed_text(argv[1]);
    if(!text.length()){
        return File_processing_error;
    }

    char32_t*         p      = const_cast<char32_t*>(text.c_str());
    auto              loc    = std::make_shared<ascaner::Location>(p);
    Errors_and_tries  et;
    et.ec_                   = std::make_shared<Error_count>();
    et.ids_trie_             = std::make_shared<Char_trie>();
    et.strs_trie_            = std::make_shared<Char_trie>();
    auto              scp    = std::make_shared<Scope>();

//     add_action(et, scp, write_act_name,          write_act_body);
//     add_action(et, scp, add_dec_digit_act_name,  add_dec_digit_act_body);
//     add_action(et, scp, add_hex_digit_act_name,  add_hex_digit_act_body);
//     add_action(et, scp, add_bin_digit_act_name,  add_bin_digit_act_body);
//     add_action(et, scp, add_oct_digit_act_name,  add_oct_digit_act_body);
//     add_action(et, scp, write_by_code_act_name,  write_by_code_act_body);
//     add_action(et, scp, add_dec_digit_act_name1, add_dec_digit_act_body1);
//     add_action(et, scp, add_hex_digit_act_name1, add_hex_digit_act_body1);
//     add_action(et, scp, add_bin_digit_act_name1, add_bin_digit_act_body1);
//     add_action(et, scp, add_oct_digit_act_name1, add_oct_digit_act_body1);

    auto              ts     = std::make_shared<Trie_for_set_of_char32>();
    auto              exprsc = std::make_shared<escaner::Expr_scaner>(loc, et, ts, scp);

    test_expr_scaner(exprsc);
    et.ec_->print();

    return Success;
}