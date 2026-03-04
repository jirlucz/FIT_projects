-- uart_rx_fsm.vhd: UART controller - finite state machine controlling RX side
-- Author(s): Jiří Kotek  (xkotek09)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



entity UART_RX_FSM is
    port(
       CLK 	: in std_logic;
       RST 	: in std_logic;
       DIN 	: in std_logic;
       CNT24 :	 in std_logic_vector (4 downto 0);
       MID_BIT 	: in std_logic_vector (4 downto 0);
       CNT8 	: in std_logic_vector (3 downto 0);
       BIT_SLC 	: in std_logic_vector (2 downto 0);
       --START : out std_logic; 
       DOUT_VLD : out std_logic; 	--Validace Dat
       RX_EN 	: out std_logic;		--Soucastka zaznamela "0" 
       DATA_EN 	: out std_logic;		--Soucastka prijima data
       clear 	: out std_logic;		--Vycisti veskerou pamet a pripravi RX na prijem
       cnt24_hold : in std_logic;
       STOP_BIT_OFFSET : in std_logic_vector (3 downto 0)
    );
end entity;



architecture behavioral of UART_RX_FSM is
    type status is (idle, start, data, stopbit, valid, reset);
    signal state : status := idle;  
begin
	RX_EN <= '1' when state = start else '0';
	DATA_EN <= '1' when state = data else '0';
	DOUT_VLD <= '1' when state = valid else '0';
	clear <= '1' when state = reset else '0';
    process (CLK) begin
        if rising_edge (CLK) then
            if RST = '1' then   --pokud RST je 1 vracime se na IDLE
                state <= idle;
            else
            	
                case state is 
                    when idle => 
                    		    if DIN = '0' then --pokud prijde start bit 0, prechazime do stavu start
                                        state <= start;
                                    end if;
                    when start => 
                                   if cnt24_hold = '1' then --cekame na midbit prvniho datoveho bitu
                                       state <= data;
                                   end if;
                    when data => 
                                    if CNT8 = "1000" then --po 8 datovych bitech koncime se ctenim dat
                                        state <= stopbit;
                                    end if;
                    when stopbit => 
                    		if DIN = '1' AND STOP_BIT_OFFSET = "1000" then --kontrola stop bitu
                                        state <= valid;
                                end if;
                    when valid =>      
                                        state <= reset;
                                        
		    when reset =>      
                                        state <= idle;   --prechod do stavu idle
			when others =>
				null;
                end case;
            end if;
        end if;
    end process;
end architecture;
