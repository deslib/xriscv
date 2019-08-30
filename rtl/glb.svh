`define OP_LUI      5'b01_101
`define OP_AUIPC    5'b00_101
`define OP_JAL      5'b11_011
`define OP_JALR     5'b11_001
`define OP_BRANCH   5'b11_000
`define OP_LOAD     5'b00_000
`define OP_STORE    5'b01_000
`define OP_IMM      5'b00_100
`define OP_REG      5'b01_100

//add or sub
`define ALU_AOS    3'b000
`define ALU_SLL    3'b001
`define ALU_SLT    3'b010
`define ALU_SLTU   3'b011
`define ALU_XOR    3'b100
`define ALU_SR     3'b101
`define ALU_OR     3'b110
`define ALU_AND    3'b111