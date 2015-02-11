add_library(shunting_yard   shunting_yard.c)
add_library(stack           stack.c)
add_library(tokenizer       tokenizer.c)
add_library(rpn_calc        rpn_calc.c)

add_executable(${EXECUTABLE_NAME} main.c)
target_link_libraries(${EXECUTABLE_NAME}
    shunting_yard
    stack
    tokenizer
    rpn_calc
)
