//
// Created by amindelavar on 9/23/2018.
//

#ifndef MPL_RUN_MGR_H
#define MPL_RUN_MGR_H

uint8 labeled_instruction(String code);
String define_vars(String inst);
String function_call(String exp, int8 status);
#endif //MPL_RUN_MGR_H