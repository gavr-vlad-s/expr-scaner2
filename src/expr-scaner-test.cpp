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
    auto              ts     = std::make_shared<Trie_for_set_of_char32>();
    auto              exprsc = std::make_shared<escaner::Expr_scaner>(loc, et, ts, scp);

    test_expr_scaner(exprsc);
    et.ec_->print();

    return Success;
}