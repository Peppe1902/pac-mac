----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 16.08.2025 18:20:05
-- Design Name: 
-- Module Name: MappingLogic - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity MappingLogic is
    Generic (
		C_S_AXI_ADDR_WIDTH	: integer	:= 40;
		WADDR_LOW : unsigned(31 downto 0) := x"50A0_0000";
		WADDR_SUP : unsigned(31 downto 0) := x"50B0_0000";
		RADDR_LOW : unsigned(31 downto 0) := x"5050_0000";
		RADDR_SUP : unsigned(31 downto 0) := x"50B0_0000"
	);
	Port (
		--CHANNEL AW--
		s_axi_awaddr	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		s_axi_awlen	    : in std_logic_vector(7 downto 0);
		s_axi_awsize	: in std_logic_vector(2 downto 0);
		s_axi_awburst	: in std_logic_vector(1 downto 0);

		--CHANNEL AW--
		s_axi_araddr	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		s_axi_arlen	: in std_logic_vector(7 downto 0);
		s_axi_arsize	: in std_logic_vector(2 downto 0);
		s_axi_arburst	: in std_logic_vector(1 downto 0);

		--OUTPUT
		mapped_axi_awaddr : out std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		mapped_axi_araddr : out std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0)
		--mapped_axi_awlen  : out std_logic_vector(7 downto 0);
		--mapped_axi_arlen  : out std_logic_vector(7 downto 0)
	);
end MappingLogic;

architecture Behavioral of MappingLogic is
    
    -- funzioni di utilità
    function calc_last_addr(
        start_addr : unsigned(31 downto 0);
        len        : std_logic_vector(7 downto 0);
        size       : std_logic_vector(2 downto 0)
    ) return unsigned is
        variable burst_len  : unsigned(7 downto 0);  -- fino a 255 beat
        variable beat_bytes : unsigned(31 downto 0);
        variable last_addr  : unsigned(31 downto 0);
    begin
        burst_len  := resize(unsigned(len), 8) + 1;
        beat_bytes := to_unsigned(2 ** to_integer(unsigned(size)), 32);
        last_addr := start_addr + resize(resize(burst_len, 32) * beat_bytes, 32) - 1;
        return last_addr;
    end function;
    
    signal aw_last,  ar_last  : unsigned(31 downto 0);
    
    begin
   
--      --Addresses Translation
--      mapped_axi_awaddr <= x"00" & s_axi_awaddr(31 downto 0) when unsigned(s_axi_awaddr(31 downto 0))>= WADDR_LOW and unsigned(s_axi_awaddr(31 downto 0)) < WADDR_SUP  else
--                            x"11" & s_axi_awaddr(31 downto 0);
--      mapped_axi_araddr <= x"00" & s_axi_araddr(31 downto 0) when unsigned(s_axi_araddr(31 downto 0)) >= RADDR_LOW  and unsigned(s_axi_araddr(31 downto 0)) < RADDR_SUP else
--                            x"11" & s_axi_araddr(31 downto 0);

    -- indirizzi finali burst
    aw_last  <= calc_last_addr(unsigned(s_axi_awaddr(31 downto 0)), s_axi_awlen, s_axi_awsize);
    ar_last  <= calc_last_addr(unsigned(s_axi_araddr(31 downto 0)), s_axi_arlen, s_axi_arsize);

    -------------------------------------------------------------------
    -- WRITE ADDRESS TRANSLATION
    -------------------------------------------------------------------
    mapped_axi_awaddr <= 
        -- valido SOLO se sia start_addr che last_addr cadono nel range
        x"00" & s_axi_awaddr(31 downto 0)
        when (unsigned(s_axi_awaddr(31 downto 0)) >= WADDR_LOW and unsigned(s_axi_awaddr(31 downto 0)) < WADDR_SUP and aw_last < WADDR_SUP)
        else
        x"11" & s_axi_awaddr(31 downto 0);


    -------------------------------------------------------------------
    -- READ ADDRESS TRANSLATION
    -------------------------------------------------------------------
    mapped_axi_araddr <= 
        -- valido SOLO se sia start_addr che last_addr cadono nel range
        x"00" & s_axi_araddr(31 downto 0)
        when (unsigned(s_axi_araddr(31 downto 0)) >= RADDR_LOW and unsigned(s_axi_araddr(31 downto 0)) < RADDR_SUP and ar_last < RADDR_SUP)
        else
        x"11" & s_axi_araddr(31 downto 0);

end Behavioral;
