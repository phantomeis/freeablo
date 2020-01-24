#pragma once

// Fixed point internal values for all 0-3 digit fractions.
// Used by a macro in fixedpoint.h
// Generated with the following python code:

/*
from itertools import combinations_with_replacement

max_digits = 3
scaling_factor_power_of_10 = 9

for digits in range(1, max_digits + 1):
    for comb in combinations_with_replacement([0,1,2,3,4,5,6,7,8,9], digits):
        number_str = ""
        for digit in comb:
            number_str += str(digit)

        scaled_value =  number_str.ljust(scaling_factor_power_of_10, '0').lstrip('0')
        if scaled_value == '':
            scaled_value = '0'

        print("#define FIXED_POINT_FRACTION_{} {}".format(number_str, scaled_value))

for int_val in range(0, 100):
    print("#define FIXED_POINT_INTPART_{} {}".format(int_val, int_val))
*/

#define FIXED_POINT_FRACTION_0 0
#define FIXED_POINT_FRACTION_1 100000000
#define FIXED_POINT_FRACTION_2 200000000
#define FIXED_POINT_FRACTION_3 300000000
#define FIXED_POINT_FRACTION_4 400000000
#define FIXED_POINT_FRACTION_5 500000000
#define FIXED_POINT_FRACTION_6 600000000
#define FIXED_POINT_FRACTION_7 700000000
#define FIXED_POINT_FRACTION_8 800000000
#define FIXED_POINT_FRACTION_9 900000000
#define FIXED_POINT_FRACTION_00 0
#define FIXED_POINT_FRACTION_01 10000000
#define FIXED_POINT_FRACTION_02 20000000
#define FIXED_POINT_FRACTION_03 30000000
#define FIXED_POINT_FRACTION_04 40000000
#define FIXED_POINT_FRACTION_05 50000000
#define FIXED_POINT_FRACTION_06 60000000
#define FIXED_POINT_FRACTION_07 70000000
#define FIXED_POINT_FRACTION_08 80000000
#define FIXED_POINT_FRACTION_09 90000000
#define FIXED_POINT_FRACTION_11 110000000
#define FIXED_POINT_FRACTION_12 120000000
#define FIXED_POINT_FRACTION_13 130000000
#define FIXED_POINT_FRACTION_14 140000000
#define FIXED_POINT_FRACTION_15 150000000
#define FIXED_POINT_FRACTION_16 160000000
#define FIXED_POINT_FRACTION_17 170000000
#define FIXED_POINT_FRACTION_18 180000000
#define FIXED_POINT_FRACTION_19 190000000
#define FIXED_POINT_FRACTION_22 220000000
#define FIXED_POINT_FRACTION_23 230000000
#define FIXED_POINT_FRACTION_24 240000000
#define FIXED_POINT_FRACTION_25 250000000
#define FIXED_POINT_FRACTION_26 260000000
#define FIXED_POINT_FRACTION_27 270000000
#define FIXED_POINT_FRACTION_28 280000000
#define FIXED_POINT_FRACTION_29 290000000
#define FIXED_POINT_FRACTION_33 330000000
#define FIXED_POINT_FRACTION_34 340000000
#define FIXED_POINT_FRACTION_35 350000000
#define FIXED_POINT_FRACTION_36 360000000
#define FIXED_POINT_FRACTION_37 370000000
#define FIXED_POINT_FRACTION_38 380000000
#define FIXED_POINT_FRACTION_39 390000000
#define FIXED_POINT_FRACTION_44 440000000
#define FIXED_POINT_FRACTION_45 450000000
#define FIXED_POINT_FRACTION_46 460000000
#define FIXED_POINT_FRACTION_47 470000000
#define FIXED_POINT_FRACTION_48 480000000
#define FIXED_POINT_FRACTION_49 490000000
#define FIXED_POINT_FRACTION_55 550000000
#define FIXED_POINT_FRACTION_56 560000000
#define FIXED_POINT_FRACTION_57 570000000
#define FIXED_POINT_FRACTION_58 580000000
#define FIXED_POINT_FRACTION_59 590000000
#define FIXED_POINT_FRACTION_66 660000000
#define FIXED_POINT_FRACTION_67 670000000
#define FIXED_POINT_FRACTION_68 680000000
#define FIXED_POINT_FRACTION_69 690000000
#define FIXED_POINT_FRACTION_77 770000000
#define FIXED_POINT_FRACTION_78 780000000
#define FIXED_POINT_FRACTION_79 790000000
#define FIXED_POINT_FRACTION_88 880000000
#define FIXED_POINT_FRACTION_89 890000000
#define FIXED_POINT_FRACTION_99 990000000
#define FIXED_POINT_FRACTION_000 0
#define FIXED_POINT_FRACTION_001 1000000
#define FIXED_POINT_FRACTION_002 2000000
#define FIXED_POINT_FRACTION_003 3000000
#define FIXED_POINT_FRACTION_004 4000000
#define FIXED_POINT_FRACTION_005 5000000
#define FIXED_POINT_FRACTION_006 6000000
#define FIXED_POINT_FRACTION_007 7000000
#define FIXED_POINT_FRACTION_008 8000000
#define FIXED_POINT_FRACTION_009 9000000
#define FIXED_POINT_FRACTION_011 11000000
#define FIXED_POINT_FRACTION_012 12000000
#define FIXED_POINT_FRACTION_013 13000000
#define FIXED_POINT_FRACTION_014 14000000
#define FIXED_POINT_FRACTION_015 15000000
#define FIXED_POINT_FRACTION_016 16000000
#define FIXED_POINT_FRACTION_017 17000000
#define FIXED_POINT_FRACTION_018 18000000
#define FIXED_POINT_FRACTION_019 19000000
#define FIXED_POINT_FRACTION_022 22000000
#define FIXED_POINT_FRACTION_023 23000000
#define FIXED_POINT_FRACTION_024 24000000
#define FIXED_POINT_FRACTION_025 25000000
#define FIXED_POINT_FRACTION_026 26000000
#define FIXED_POINT_FRACTION_027 27000000
#define FIXED_POINT_FRACTION_028 28000000
#define FIXED_POINT_FRACTION_029 29000000
#define FIXED_POINT_FRACTION_033 33000000
#define FIXED_POINT_FRACTION_034 34000000
#define FIXED_POINT_FRACTION_035 35000000
#define FIXED_POINT_FRACTION_036 36000000
#define FIXED_POINT_FRACTION_037 37000000
#define FIXED_POINT_FRACTION_038 38000000
#define FIXED_POINT_FRACTION_039 39000000
#define FIXED_POINT_FRACTION_044 44000000
#define FIXED_POINT_FRACTION_045 45000000
#define FIXED_POINT_FRACTION_046 46000000
#define FIXED_POINT_FRACTION_047 47000000
#define FIXED_POINT_FRACTION_048 48000000
#define FIXED_POINT_FRACTION_049 49000000
#define FIXED_POINT_FRACTION_055 55000000
#define FIXED_POINT_FRACTION_056 56000000
#define FIXED_POINT_FRACTION_057 57000000
#define FIXED_POINT_FRACTION_058 58000000
#define FIXED_POINT_FRACTION_059 59000000
#define FIXED_POINT_FRACTION_066 66000000
#define FIXED_POINT_FRACTION_067 67000000
#define FIXED_POINT_FRACTION_068 68000000
#define FIXED_POINT_FRACTION_069 69000000
#define FIXED_POINT_FRACTION_077 77000000
#define FIXED_POINT_FRACTION_078 78000000
#define FIXED_POINT_FRACTION_079 79000000
#define FIXED_POINT_FRACTION_088 88000000
#define FIXED_POINT_FRACTION_089 89000000
#define FIXED_POINT_FRACTION_099 99000000
#define FIXED_POINT_FRACTION_111 111000000
#define FIXED_POINT_FRACTION_112 112000000
#define FIXED_POINT_FRACTION_113 113000000
#define FIXED_POINT_FRACTION_114 114000000
#define FIXED_POINT_FRACTION_115 115000000
#define FIXED_POINT_FRACTION_116 116000000
#define FIXED_POINT_FRACTION_117 117000000
#define FIXED_POINT_FRACTION_118 118000000
#define FIXED_POINT_FRACTION_119 119000000
#define FIXED_POINT_FRACTION_122 122000000
#define FIXED_POINT_FRACTION_123 123000000
#define FIXED_POINT_FRACTION_124 124000000
#define FIXED_POINT_FRACTION_125 125000000
#define FIXED_POINT_FRACTION_126 126000000
#define FIXED_POINT_FRACTION_127 127000000
#define FIXED_POINT_FRACTION_128 128000000
#define FIXED_POINT_FRACTION_129 129000000
#define FIXED_POINT_FRACTION_133 133000000
#define FIXED_POINT_FRACTION_134 134000000
#define FIXED_POINT_FRACTION_135 135000000
#define FIXED_POINT_FRACTION_136 136000000
#define FIXED_POINT_FRACTION_137 137000000
#define FIXED_POINT_FRACTION_138 138000000
#define FIXED_POINT_FRACTION_139 139000000
#define FIXED_POINT_FRACTION_144 144000000
#define FIXED_POINT_FRACTION_145 145000000
#define FIXED_POINT_FRACTION_146 146000000
#define FIXED_POINT_FRACTION_147 147000000
#define FIXED_POINT_FRACTION_148 148000000
#define FIXED_POINT_FRACTION_149 149000000
#define FIXED_POINT_FRACTION_155 155000000
#define FIXED_POINT_FRACTION_156 156000000
#define FIXED_POINT_FRACTION_157 157000000
#define FIXED_POINT_FRACTION_158 158000000
#define FIXED_POINT_FRACTION_159 159000000
#define FIXED_POINT_FRACTION_166 166000000
#define FIXED_POINT_FRACTION_167 167000000
#define FIXED_POINT_FRACTION_168 168000000
#define FIXED_POINT_FRACTION_169 169000000
#define FIXED_POINT_FRACTION_177 177000000
#define FIXED_POINT_FRACTION_178 178000000
#define FIXED_POINT_FRACTION_179 179000000
#define FIXED_POINT_FRACTION_188 188000000
#define FIXED_POINT_FRACTION_189 189000000
#define FIXED_POINT_FRACTION_199 199000000
#define FIXED_POINT_FRACTION_222 222000000
#define FIXED_POINT_FRACTION_223 223000000
#define FIXED_POINT_FRACTION_224 224000000
#define FIXED_POINT_FRACTION_225 225000000
#define FIXED_POINT_FRACTION_226 226000000
#define FIXED_POINT_FRACTION_227 227000000
#define FIXED_POINT_FRACTION_228 228000000
#define FIXED_POINT_FRACTION_229 229000000
#define FIXED_POINT_FRACTION_233 233000000
#define FIXED_POINT_FRACTION_234 234000000
#define FIXED_POINT_FRACTION_235 235000000
#define FIXED_POINT_FRACTION_236 236000000
#define FIXED_POINT_FRACTION_237 237000000
#define FIXED_POINT_FRACTION_238 238000000
#define FIXED_POINT_FRACTION_239 239000000
#define FIXED_POINT_FRACTION_244 244000000
#define FIXED_POINT_FRACTION_245 245000000
#define FIXED_POINT_FRACTION_246 246000000
#define FIXED_POINT_FRACTION_247 247000000
#define FIXED_POINT_FRACTION_248 248000000
#define FIXED_POINT_FRACTION_249 249000000
#define FIXED_POINT_FRACTION_255 255000000
#define FIXED_POINT_FRACTION_256 256000000
#define FIXED_POINT_FRACTION_257 257000000
#define FIXED_POINT_FRACTION_258 258000000
#define FIXED_POINT_FRACTION_259 259000000
#define FIXED_POINT_FRACTION_266 266000000
#define FIXED_POINT_FRACTION_267 267000000
#define FIXED_POINT_FRACTION_268 268000000
#define FIXED_POINT_FRACTION_269 269000000
#define FIXED_POINT_FRACTION_277 277000000
#define FIXED_POINT_FRACTION_278 278000000
#define FIXED_POINT_FRACTION_279 279000000
#define FIXED_POINT_FRACTION_288 288000000
#define FIXED_POINT_FRACTION_289 289000000
#define FIXED_POINT_FRACTION_299 299000000
#define FIXED_POINT_FRACTION_333 333000000
#define FIXED_POINT_FRACTION_334 334000000
#define FIXED_POINT_FRACTION_335 335000000
#define FIXED_POINT_FRACTION_336 336000000
#define FIXED_POINT_FRACTION_337 337000000
#define FIXED_POINT_FRACTION_338 338000000
#define FIXED_POINT_FRACTION_339 339000000
#define FIXED_POINT_FRACTION_344 344000000
#define FIXED_POINT_FRACTION_345 345000000
#define FIXED_POINT_FRACTION_346 346000000
#define FIXED_POINT_FRACTION_347 347000000
#define FIXED_POINT_FRACTION_348 348000000
#define FIXED_POINT_FRACTION_349 349000000
#define FIXED_POINT_FRACTION_355 355000000
#define FIXED_POINT_FRACTION_356 356000000
#define FIXED_POINT_FRACTION_357 357000000
#define FIXED_POINT_FRACTION_358 358000000
#define FIXED_POINT_FRACTION_359 359000000
#define FIXED_POINT_FRACTION_366 366000000
#define FIXED_POINT_FRACTION_367 367000000
#define FIXED_POINT_FRACTION_368 368000000
#define FIXED_POINT_FRACTION_369 369000000
#define FIXED_POINT_FRACTION_377 377000000
#define FIXED_POINT_FRACTION_378 378000000
#define FIXED_POINT_FRACTION_379 379000000
#define FIXED_POINT_FRACTION_388 388000000
#define FIXED_POINT_FRACTION_389 389000000
#define FIXED_POINT_FRACTION_399 399000000
#define FIXED_POINT_FRACTION_444 444000000
#define FIXED_POINT_FRACTION_445 445000000
#define FIXED_POINT_FRACTION_446 446000000
#define FIXED_POINT_FRACTION_447 447000000
#define FIXED_POINT_FRACTION_448 448000000
#define FIXED_POINT_FRACTION_449 449000000
#define FIXED_POINT_FRACTION_455 455000000
#define FIXED_POINT_FRACTION_456 456000000
#define FIXED_POINT_FRACTION_457 457000000
#define FIXED_POINT_FRACTION_458 458000000
#define FIXED_POINT_FRACTION_459 459000000
#define FIXED_POINT_FRACTION_466 466000000
#define FIXED_POINT_FRACTION_467 467000000
#define FIXED_POINT_FRACTION_468 468000000
#define FIXED_POINT_FRACTION_469 469000000
#define FIXED_POINT_FRACTION_477 477000000
#define FIXED_POINT_FRACTION_478 478000000
#define FIXED_POINT_FRACTION_479 479000000
#define FIXED_POINT_FRACTION_488 488000000
#define FIXED_POINT_FRACTION_489 489000000
#define FIXED_POINT_FRACTION_499 499000000
#define FIXED_POINT_FRACTION_555 555000000
#define FIXED_POINT_FRACTION_556 556000000
#define FIXED_POINT_FRACTION_557 557000000
#define FIXED_POINT_FRACTION_558 558000000
#define FIXED_POINT_FRACTION_559 559000000
#define FIXED_POINT_FRACTION_566 566000000
#define FIXED_POINT_FRACTION_567 567000000
#define FIXED_POINT_FRACTION_568 568000000
#define FIXED_POINT_FRACTION_569 569000000
#define FIXED_POINT_FRACTION_577 577000000
#define FIXED_POINT_FRACTION_578 578000000
#define FIXED_POINT_FRACTION_579 579000000
#define FIXED_POINT_FRACTION_588 588000000
#define FIXED_POINT_FRACTION_589 589000000
#define FIXED_POINT_FRACTION_599 599000000
#define FIXED_POINT_FRACTION_666 666000000
#define FIXED_POINT_FRACTION_667 667000000
#define FIXED_POINT_FRACTION_668 668000000
#define FIXED_POINT_FRACTION_669 669000000
#define FIXED_POINT_FRACTION_677 677000000
#define FIXED_POINT_FRACTION_678 678000000
#define FIXED_POINT_FRACTION_679 679000000
#define FIXED_POINT_FRACTION_688 688000000
#define FIXED_POINT_FRACTION_689 689000000
#define FIXED_POINT_FRACTION_699 699000000
#define FIXED_POINT_FRACTION_777 777000000
#define FIXED_POINT_FRACTION_778 778000000
#define FIXED_POINT_FRACTION_779 779000000
#define FIXED_POINT_FRACTION_788 788000000
#define FIXED_POINT_FRACTION_789 789000000
#define FIXED_POINT_FRACTION_799 799000000
#define FIXED_POINT_FRACTION_888 888000000
#define FIXED_POINT_FRACTION_889 889000000
#define FIXED_POINT_FRACTION_899 899000000
#define FIXED_POINT_FRACTION_999 999000000
#define FIXED_POINT_INTPART_0 0
#define FIXED_POINT_INTPART_1 1
#define FIXED_POINT_INTPART_2 2
#define FIXED_POINT_INTPART_3 3
#define FIXED_POINT_INTPART_4 4
#define FIXED_POINT_INTPART_5 5
#define FIXED_POINT_INTPART_6 6
#define FIXED_POINT_INTPART_7 7
#define FIXED_POINT_INTPART_8 8
#define FIXED_POINT_INTPART_9 9
#define FIXED_POINT_INTPART_10 10
#define FIXED_POINT_INTPART_11 11
#define FIXED_POINT_INTPART_12 12
#define FIXED_POINT_INTPART_13 13
#define FIXED_POINT_INTPART_14 14
#define FIXED_POINT_INTPART_15 15
#define FIXED_POINT_INTPART_16 16
#define FIXED_POINT_INTPART_17 17
#define FIXED_POINT_INTPART_18 18
#define FIXED_POINT_INTPART_19 19
#define FIXED_POINT_INTPART_20 20
#define FIXED_POINT_INTPART_21 21
#define FIXED_POINT_INTPART_22 22
#define FIXED_POINT_INTPART_23 23
#define FIXED_POINT_INTPART_24 24
#define FIXED_POINT_INTPART_25 25
#define FIXED_POINT_INTPART_26 26
#define FIXED_POINT_INTPART_27 27
#define FIXED_POINT_INTPART_28 28
#define FIXED_POINT_INTPART_29 29
#define FIXED_POINT_INTPART_30 30
#define FIXED_POINT_INTPART_31 31
#define FIXED_POINT_INTPART_32 32
#define FIXED_POINT_INTPART_33 33
#define FIXED_POINT_INTPART_34 34
#define FIXED_POINT_INTPART_35 35
#define FIXED_POINT_INTPART_36 36
#define FIXED_POINT_INTPART_37 37
#define FIXED_POINT_INTPART_38 38
#define FIXED_POINT_INTPART_39 39
#define FIXED_POINT_INTPART_40 40
#define FIXED_POINT_INTPART_41 41
#define FIXED_POINT_INTPART_42 42
#define FIXED_POINT_INTPART_43 43
#define FIXED_POINT_INTPART_44 44
#define FIXED_POINT_INTPART_45 45
#define FIXED_POINT_INTPART_46 46
#define FIXED_POINT_INTPART_47 47
#define FIXED_POINT_INTPART_48 48
#define FIXED_POINT_INTPART_49 49
#define FIXED_POINT_INTPART_50 int64_t(50)
#define FIXED_POINT_INTPART_51 51
#define FIXED_POINT_INTPART_52 52
#define FIXED_POINT_INTPART_53 53
#define FIXED_POINT_INTPART_54 54
#define FIXED_POINT_INTPART_55 55
#define FIXED_POINT_INTPART_56 56
#define FIXED_POINT_INTPART_57 57
#define FIXED_POINT_INTPART_58 58
#define FIXED_POINT_INTPART_59 59
#define FIXED_POINT_INTPART_60 60
#define FIXED_POINT_INTPART_61 61
#define FIXED_POINT_INTPART_62 62
#define FIXED_POINT_INTPART_63 63
#define FIXED_POINT_INTPART_64 64
#define FIXED_POINT_INTPART_65 65
#define FIXED_POINT_INTPART_66 66
#define FIXED_POINT_INTPART_67 67
#define FIXED_POINT_INTPART_68 68
#define FIXED_POINT_INTPART_69 69
#define FIXED_POINT_INTPART_70 70
#define FIXED_POINT_INTPART_71 71
#define FIXED_POINT_INTPART_72 72
#define FIXED_POINT_INTPART_73 73
#define FIXED_POINT_INTPART_74 74
#define FIXED_POINT_INTPART_75 75
#define FIXED_POINT_INTPART_76 76
#define FIXED_POINT_INTPART_77 77
#define FIXED_POINT_INTPART_78 78
#define FIXED_POINT_INTPART_79 79
#define FIXED_POINT_INTPART_80 80
#define FIXED_POINT_INTPART_81 81
#define FIXED_POINT_INTPART_82 82
#define FIXED_POINT_INTPART_83 83
#define FIXED_POINT_INTPART_84 84
#define FIXED_POINT_INTPART_85 85
#define FIXED_POINT_INTPART_86 86
#define FIXED_POINT_INTPART_87 87
#define FIXED_POINT_INTPART_88 88
#define FIXED_POINT_INTPART_89 89
#define FIXED_POINT_INTPART_90 90
#define FIXED_POINT_INTPART_91 91
#define FIXED_POINT_INTPART_92 92
#define FIXED_POINT_INTPART_93 93
#define FIXED_POINT_INTPART_94 94
#define FIXED_POINT_INTPART_95 95
#define FIXED_POINT_INTPART_96 96
#define FIXED_POINT_INTPART_97 97
#define FIXED_POINT_INTPART_98 98
#define FIXED_POINT_INTPART_99 99
