#include <stdio.h>
#include "rule.h"

int main(int argc, char* argv[])
{
    Param args;
    args.type = type_string;
    args.values.sData = argv[1];
    printf("result is %d\n", RuleCheck::getInstance()->doCheck(1, 1, &args));
    return 0;
}

