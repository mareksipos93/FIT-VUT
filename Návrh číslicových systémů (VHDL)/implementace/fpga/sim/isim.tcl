# sim.tcl : ISIM simulation script
# Copyright (C) 2011 Brno University of Technology,
#                    Faculty of Information Technology
# Author(s): Zdenek Vasicek
#
# LICENSE TERMS
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. All advertising materials mentioning features or use of this software
#    or firmware must display the following acknowledgement:
#
#      This product includes software developed by the University of
#      Technology, Faculty of Information Technology, Brno and its
#      contributors.
#
# 4. Neither the name of the Company nor the names of its contributors
#    may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# This software or firmware is provided ``as is'', and any express or implied
# warranties, including, but not limited to, the implied warranties of
# merchantability and fitness for a particular purpose are disclaimed.
# In no event shall the company or contributors be liable for any
# direct, indirect, incidental, special, exemplary, or consequential
# damages (including, but not limited to, procurement of substitute
# goods or services; loss of use, data, or profits; or business
# interruption) however caused and on any theory of liability, whether
# in contract, strict liability, or tort (including negligence or
# otherwise) arising in any way out of the use of this software, even
# if advised of the possibility of such damage.
#

#Project setup
#========================================
#set TESTBENCH_ENTITY "testbench"
#set ISIM_PRECISION "100 ps"

#Run simulation
#========================================
proc isim_script {} {

	wave add -name reset /testbench/uut/cnt_reset_ce
	wave add -name clk /testbench/uut/smclk

	# Keyboard signals --------------------------------------------
	divider add "Keyboard Interface"
	wave add -name key /testbench/key
	wave add -name kin /testbench/uut/kin
	wave add -name kout /testbench/uut/kout

	# LCD display signals -----------------------------------------
	divider add "LCD Display Interface"
	wave add -name lrw /testbench/uut/lrw
	wave add -name le /testbench/uut/le
	wave add -name lrs /testbench/uut/lrs
	wave add -radix hex -name ld /testbench/uut/ld
	wave add -name display /testbench/lcd_u/display

	# User signals -----------------------------------------------
	divider add "Keyboard Controller"
	wave add -name clk /testbench/uut/kbc_u/clk 
	wave add -name data_out /testbench/uut/kbc_u/data_out 

	divider add "LCD Controller"
	wave add -name clk /testbench/uut/lcdc_u/clk
	wave add -radix ascii -name data /testbench/uut/lcdc_u/data
	wave add -name write /testbench/uut/lcdc_u/write
	wave add -name clear /testbench/uut/lcdc_u/clear

	divider add "FSM Signals"
	wave add -name clk /testbench/uut/fsm_u/clk
	wave add -name key /testbench/uut/fsm_u/key
	wave add -name cnt_of /testbench/uut/fsm_u/cnt_of
	wave add -name fsm_cnt_ce /testbench/uut/fsm_u/fsm_cnt_ce
	wave add -name fsm_lcd_clr /testbench/uut/fsm_u/fsm_lcd_clr
	wave add -name fsm_lcd_wr /testbench/uut/fsm_u/fsm_lcd_wr
	wave add -name fsm_mx_mem /testbench/uut/fsm_u/fsm_mx_mem
	wave add -name fsm_mx_lcd /testbench/uut/fsm_u/fsm_mx_lcd
	wave add -name present_state /testbench/uut/fsm_u/present_state

	divider add "ROM Memory Signals"
	wave add -name clk /testbench/uut/clk
	wave add -name addr /testbench/uut/rom_ok/addr
	wave add -radix ascii -name data_out /testbench/uut/rom_ok/data_out
	wave add -name addr /testbench/uut/rom_bad/addr
	wave add -radix ascii -name data_out /testbench/uut/rom_bad/data_out

	divider add "FPGA Logic"
	wave add -name clk /testbench/uut/clk
	wave add -name cnt_mem_ld /testbench/uut/cnt_mem_ld
	wave add -name cnt_mem_ce /testbench/uut/cnt_mem_ce
	wave add -name cnt_mem /testbench/uut/cnt_mem
	wave add -name cnt_mem_of /testbench/uut/cnt_mem_of
	wave add -radix ascii -name mx_mem_do /testbench/uut/mx_mem_do
	wave add -radix ascii -name mx_lcd_data /testbench/uut/mx_lcd_data
   
   	run 200 us
} 

