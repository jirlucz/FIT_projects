-- uart_rx.vhd: UART controller - receiving (RX) side
-- Author(s): Jiří Kotek (xkotek09)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



-- Entity declaration (DO NOT ALTER THIS PART!)
entity UART_RX is
    port(
        CLK      : in std_logic;
        RST      : in std_logic;
        DIN      : in std_logic;
        DOUT_VLD : out std_logic;
        DOUT     : out std_logic_vector(7 downto 0)
    );
end entity;



-- Architecture implementation (INSERT YOUR IMPLEMENTATION HERE)
architecture behavioral of UART_RX is
    signal cnt24 : 	std_logic_vector (4 downto 0) := "00000";
    signal MID_BIT : 	std_logic_vector (4 downto 0) := "00000";
    signal cnt8 : 	std_logic_vector (3 downto 0) := "0000";
    signal BIT_SLC : 	std_logic_vector (2 downto 0) := "000";
    signal RX_EN : 	std_logic := '0';
    signal DATA_EN :	std_logic := '0';
    signal cnt24_hold :	std_logic := '0';
    signal mid_bit_reset :	std_logic := '0';
    signal STOP_BIT_OFFSET : std_logic_vector (3 downto 0) := "0000";
    signal clear : std_logic := '0';

begin

    -- Instance of RX FSM
    fsm: entity work.UART_RX_FSM
    port map (
        CLK 	=> CLK,
        RST 	=> RST,
        DIN 	=> din,
        CNT24 	=> cnt24,
        CNT8 	=> cnt8,
        RX_EN 	=> RX_EN,
        DOUT_VLD => DOUT_VLD,
        MID_BIT => MID_BIT,
        BIT_SLC => BIT_SLC,
        cnt24_hold => cnt24_hold,
        DATA_EN => DATA_EN,
        STOP_BIT_OFFSET => STOP_BIT_OFFSET,
        clear => clear
    );

    --DOUT <= (others => '0');
    --DOUT_VLD <= '0';

    p_cnt24 : process (CLK) begin
    	if RX_EN = '1' then
    		if rising_edge (CLK) then
    		
		    	if cnt24_hold = '0' then
		    		cnt24 <= cnt24 + 1;
		    	end if;
		end if;
	end if;
	if clear = '1' then
		cnt24 <= "00000";
	end if;
	--end if;
    end process;
    
    p_cmp24 : process (CLK) begin
    	if CNT24 = "10110" then
    		cnt24_hold <= '1';
    		if RX_EN = '1' then
    		DOUT (0) <= DIN;
    		end if;
    	else
    		cnt24_hold <= '0';
    	end if;
    end process;
    
    p_mid_bit : process (CLK) begin
    	if DATA_EN = '1' then
    		if rising_edge (CLK) then
    		
		    	if MID_BIT_reset = '0' then
		    		MID_BIT <= MID_BIT + 1;
		    	else 
		    		MID_BIT <= "00000";
		    	end if;
		end if;
	end if;
	if clear = '1' then
		MID_BIT <= "00000";
	end if;
    end process;
    
    p_cmp16 : process (CLK) begin
    	if MID_BIT = "01111" then
    		MID_BIT_reset <= '1';
    	else
    		MID_BIT_reset <= '0';
    	end if;
    end process;
    
    p_bit_slc : process (CLK) begin
    	if DATA_EN = '1' then
    		if rising_edge (CLK) then
    			if MID_BIT_reset = '1' then
    				BIT_SLC <= BIT_SLC + 1;
    			end if;
		end if;
	end if;
	if clear = '1' then
		BIT_SLC <= "000";
	end if;
    end process;
    
    p_bit_cnt : process (CLK) begin
    	if DATA_EN = '1' then
    		if rising_edge (CLK) then
    			if MID_BIT_reset = '1' then
    				CNT8 <= CNT8 + 1;
    			end if;
		end if;
	end if;
	if clear = '1' then
		CNT8 <= "0000";
	end if;
    end process;
    
    p_DMX : process (CLK) begin
    	
    	case BIT_SLC is
    		when "000"=>
    			if MID_BIT = "01111" then
    				DOUT (1) <= DIN;
    			end if;
    		when "001"=>
    			if MID_BIT = "01111" then
    				DOUT (2) <= DIN;
    			end if;
    		when "010"=>
    			if MID_BIT = "01111" then
    				DOUT (3) <= DIN;
    			end if;
    		when "011"=>
    			if MID_BIT = "01111" then
    				DOUT (4) <= DIN;
    			end if;
    		when "100"=>
    			if MID_BIT = "01111" then
    				DOUT (5) <= DIN;
    			end if;
    		when "101"=>
    			if MID_BIT = "01111" then
    				DOUT (6) <= DIN;
    			end if;
    		when "110"=>
    			if MID_BIT = "01111" then
    				DOUT (7) <= DIN;
    			end if;
    		when others =>
    			null;
    	end case;
    	

    end process;
    
    p_STOP_BIT_OFFSET : process (CLK) begin
    	if CNT8 = "1000" then
    		if rising_edge (CLK) then
    			STOP_BIT_OFFSET <= STOP_BIT_OFFSET+1;
		end if;
	if clear = '1' then
		STOP_BIT_OFFSET <= "0000";
		end if;
	end if;
    end process;
    

end architecture;
