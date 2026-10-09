library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity SINK is
	generic (
		-- Users to add parameters here

		-- User parameters ends
		-- Do not modify the parameters beyond this line


		-- Parameters of Axi Slave Bus Interface S_AXI
		C_S_AXI_ID_WIDTH	: integer	:= 0;
		C_S_AXI_DATA_WIDTH	: integer	:= 32;
		C_S_AXI_ADDR_WIDTH	: integer	:= 40;
		C_S_AXI_AWUSER_WIDTH	: integer	:= 0;
		C_S_AXI_ARUSER_WIDTH	: integer	:= 0;
		C_S_AXI_WUSER_WIDTH	: integer	:= 0;
		C_S_AXI_RUSER_WIDTH	: integer	:= 0;
		C_S_AXI_BUSER_WIDTH	: integer	:= 0
	);
	port (
		-- Users to add ports here

		-- User ports ends
		-- Do not modify the ports beyond this line


		-- Ports of Axi Slave Bus Interface S_AXI
		s_axi_aclk	: in std_logic;
		s_axi_aresetn	: in std_logic;
		s_axi_awid	: in std_logic_vector(C_S_AXI_ID_WIDTH-1 downto 0);
		s_axi_awaddr	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		s_axi_awlen	: in std_logic_vector(7 downto 0);
		s_axi_awsize	: in std_logic_vector(2 downto 0);
		s_axi_awburst	: in std_logic_vector(1 downto 0);
		s_axi_awlock	: in std_logic;
		s_axi_awcache	: in std_logic_vector(3 downto 0);
		s_axi_awprot	: in std_logic_vector(2 downto 0);
		s_axi_awqos	: in std_logic_vector(3 downto 0);
		s_axi_awregion	: in std_logic_vector(3 downto 0);
		s_axi_awuser	: in std_logic_vector(C_S_AXI_AWUSER_WIDTH-1 downto 0);
		s_axi_awvalid	: in std_logic;
		s_axi_awready	: out std_logic;
		s_axi_wdata	: in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		s_axi_wstrb	: in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
		s_axi_wlast	: in std_logic;
		s_axi_wuser	: in std_logic_vector(C_S_AXI_WUSER_WIDTH-1 downto 0);
		s_axi_wvalid	: in std_logic;
		s_axi_wready	: out std_logic;
		s_axi_bid	: out std_logic_vector(C_S_AXI_ID_WIDTH-1 downto 0);
		s_axi_bresp	: out std_logic_vector(1 downto 0);
		s_axi_buser	: out std_logic_vector(C_S_AXI_BUSER_WIDTH-1 downto 0);
		s_axi_bvalid	: out std_logic;
		s_axi_bready	: in std_logic;
		s_axi_arid	: in std_logic_vector(C_S_AXI_ID_WIDTH-1 downto 0);
		s_axi_araddr	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		s_axi_arlen	: in std_logic_vector(7 downto 0);
		s_axi_arsize	: in std_logic_vector(2 downto 0);
		s_axi_arburst	: in std_logic_vector(1 downto 0);
		s_axi_arlock	: in std_logic;
		s_axi_arcache	: in std_logic_vector(3 downto 0);
		s_axi_arprot	: in std_logic_vector(2 downto 0);
		s_axi_arqos	: in std_logic_vector(3 downto 0);
		s_axi_arregion	: in std_logic_vector(3 downto 0);
		s_axi_aruser	: in std_logic_vector(C_S_AXI_ARUSER_WIDTH-1 downto 0);
		s_axi_arvalid	: in std_logic;
		s_axi_arready	: out std_logic;
		s_axi_rid	: out std_logic_vector(C_S_AXI_ID_WIDTH-1 downto 0);
		s_axi_rdata	: out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		s_axi_rresp	: out std_logic_vector(1 downto 0);
		s_axi_rlast	: out std_logic;
		s_axi_ruser	: out std_logic_vector(C_S_AXI_RUSER_WIDTH-1 downto 0);
		s_axi_rvalid	: out std_logic;
		s_axi_rready	: in std_logic
	);
end SINK;

architecture arch_imp of SINK is

    signal s_axi_trans_awaddr: std_logic_vector(31 downto 0) := (others => '0');
    signal s_axi_trans_araddr: std_logic_vector(31 downto 0) := (others => '0');
    
    type stato_w is (IDLE_W,WADDR,CONSUME_BEATS_W,SEND_BRESP_W);
    
    type stato_r is (IDLE_R,RADDR,SEND_RRESP);
    
    signal stato_corrente_w : stato_w := IDLE_W;
    signal stato_corrente_r : stato_r := IDLE_R;
    
    signal s_axi_arid_tmp : std_logic_vector(C_S_AXI_ID_WIDTH-1 downto 0) := (others => '0');
    signal s_axi_awid_tmp : std_logic_vector(C_S_AXI_ID_WIDTH-1 downto 0) := (others => '0');
    
    signal AXI_AWREADY : std_logic := '0';
    signal AXI_WREADY : std_logic := '0';
    signal AXI_BVALID : std_logic := '0';
    signal AXI_ARREADY : std_logic := '0';
    signal AXI_RVALID : std_logic := '0';
    signal AXI_RLAST : std_logic := '0';
   
    begin 
	  -- Add user logic here
    
        s_axi_awready <= AXI_AWREADY;
        s_axi_arready <= AXI_ARREADY;
        s_axi_bvalid <= AXI_BVALID;
        s_axi_rvalid <= AXI_RVALID;
        s_axi_rlast <= AXI_RLAST;
	    s_axi_wready <= AXI_WREADY;
        
    
        ----------------------------------------------------TRANSAZIONE SCRITTURA-------------------------------------------------
        STATO_PROSSIMO_W : process(s_axi_aclk) 
        
            begin
          
                if rising_edge(s_axi_aclk) then
               
                    if s_axi_aresetn = '0' then
                   
                            stato_corrente_w   <= IDLE_W;
                            
                            AXI_AWREADY <= '0';
                            AXI_WREADY  <= '0';
                            AXI_BVALID <= '0';
                            s_axi_bresp   <= (others => '0');
                            s_axi_bid     <= (others => '0');
                
                    else 
                        case (stato_corrente_w) is 
                        
                            when IDLE_W =>    
                                AXI_AWREADY <= '0';
                                AXI_WREADY  <= '0';
                                AXI_BVALID <= '0';
                                s_axi_bresp   <= (others => '0');
                                s_axi_bid     <= (others => '0');
                    
                                if s_axi_aresetn = '1' then --reset rilasciato
                                       stato_corrente_w <= WADDR;
                                else
                                       stato_corrente_w <= IDLE_W;
                                end if;
                                   
                        
                            when WADDR => --STATO INIZIALE (ATTENDO DI RICEVERE AWADDR)
                                AXI_AWREADY <= '1';
                                AXI_WREADY <= '0';
                                
                                if s_axi_awvalid = '1' and AXI_AWREADY = '1' then --handshake AXI channel AW
                                    s_axi_awid_tmp <= s_axi_awid;
                                    stato_corrente_w <= CONSUME_BEATS_W;
                                else 
                                    stato_corrente_w <= WADDR;
                                end if;
   
                            
                             when CONSUME_BEATS_W => --STATO IN CI RIMANGO FINO A CHE WLAST NON RIVENTA 1
                                AXI_WREADY <= '1'; --mantengo WREADY alto
                                AXI_AWREADY <= '0'; --abbasso AWREADY 
                                if s_axi_wlast = '1' and s_axi_wvalid='1' then
                                    stato_corrente_w <= SEND_BRESP_W;
                                elsif s_axi_wlast='0' and s_axi_wvalid='1' then
                                    stato_corrente_w <= CONSUME_BEATS_W;
                                end if;
                               
                            when SEND_BRESP_W => --STATO PER INVIARE UN MESSAGGIO DI ERRORE
                                AXI_AWREADY <= '0';
                                AXI_WREADY <= '0';
                                AXI_BVALID <= '1';
                                s_axi_bid <= s_axi_awid_tmp;
                                s_axi_bresp <= "10";
                                if s_axi_bready = '1' and AXI_BVALID = '1' then
                                    AXI_BVALID <= '0';   -- abbasso BVALID solo dopo il handshake
                                    stato_corrente_w <= IDLE_W;
                                else
                                    stato_corrente_w <= SEND_BRESP_W;
                                end if;
                                
                             when others =>
                                stato_corrente_w <= IDLE_W;
                                
                        end case;
                     end if;
                 end if;
            end process;
        
        
        
        
        
        -------------------------------------------------------TRANSAZIONE LETTURA----------------------------------------------------------
        STATO_PROSSIMO_R : process(s_axi_aclk) 
        
            begin
         
                if rising_edge(s_axi_aclk) then
                                
                        if s_axi_aresetn = '0' then
                    
                            stato_corrente_r   <= IDLE_R;
                            
                             -- CHANNEL AR
                            AXI_ARREADY   <=  '0';
                                
                            
                            -- CHANNEL R
                            s_axi_rid       <=  (others => '0');  
                            s_axi_rdata	    <=  (others => '0');  
                            s_axi_rresp	    <=  (others => '0');   
                            AXI_RLAST	    <=  '0';   
                            AXI_RVALID   <=  '0';
                            
                      else
                        case (stato_corrente_r) is 
                        
                            when IDLE_R =>    

                                 -- CHANNEL AR
                                AXI_ARREADY   <=  '0';
                                    
                                
                                -- CHANNEL R
                                s_axi_rid       <=  (others => '0');  
                                s_axi_rdata	    <=  (others => '0');  
                                s_axi_rresp	    <=  (others => '0');   
                                AXI_RLAST	    <=  '0';   
                                AXI_RVALID   <=  '0';
                                                    
                                if s_axi_aresetn = '1' then --reset rilasciato
                                      stato_corrente_r <= RADDR;
                                else
                                      stato_corrente_r <= IDLE_R;
                                end if;
                               
                        
                            when RADDR => 
                                AXI_ARREADY <= '1';
                                
                                if s_axi_arvalid = '1' and AXI_ARREADY='1' then 
                                    s_axi_arid_tmp  <= s_axi_arid;
                                    stato_corrente_r <= SEND_RRESP;
                                else 
                                    stato_corrente_r <= RADDR;
                                end if;
                                   

                            when SEND_RRESP => --STATO PER INVIARE UN MESSAGGIO DI ERRORE
                                AXI_ARREADY <= '0';
                                AXI_RVALID <= '1'; 
                                s_axi_rid <= s_axi_arid_tmp;--RID : Serve a dire a quale richiesta di lettura corrisponde quel dato.
                                --ARID : Serve a identificare una richiesta di lettura.
                                s_axi_rresp <= "10";
                                AXI_RLAST <= '1';
                                s_axi_rdata <= (others => '0');
                                
                                if s_axi_rready = '1' and AXI_RVALID='1' and AXI_RLAST = '1' then  --se r_valid=1 r_ready=1 r_last=1 allora transazione lettura è finita
                                    AXI_RVALID <= '0';
                                    stato_corrente_r <= IDLE_R;
                                else
                                    stato_corrente_r <= SEND_RRESP;
                                end if;
                                
                             when others =>
                                stato_corrente_r <= IDLE_R;
                                
                        end case;
                     end if;
                 end if;
        end process;
	-- User logic ends
end arch_imp;
