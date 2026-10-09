library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity ATL_v1_0 is
	generic (

		-- Parameters of Axi Slave Bus Interface S_AXI
		C_S_AXI_ID_WIDTH	: integer	:= 1;
		C_S_AXI_DATA_WIDTH	: integer	:= 32;
		C_S_AXI_ADDR_WIDTH	: integer	:= 40;
		C_S_AXI_AWUSER_WIDTH	: integer	:= 0;
		C_S_AXI_ARUSER_WIDTH	: integer	:= 0;
		C_S_AXI_WUSER_WIDTH	: integer	:= 0;
		C_S_AXI_RUSER_WIDTH	: integer	:= 0;
		C_S_AXI_BUSER_WIDTH	: integer	:= 0;

		-- Parameters of Axi Master Bus Interface M_AXI
		C_M_AXI_BURST_LEN	: integer	:= 16;
		C_M_AXI_ID_WIDTH	: integer	:= 1;
		C_M_AXI_ADDR_WIDTH	: integer	:= 32;
		C_M_AXI_DATA_WIDTH	: integer	:= 32;
		C_M_AXI_AWUSER_WIDTH	: integer	:= 0;
		C_M_AXI_ARUSER_WIDTH	: integer	:= 0;
		C_M_AXI_WUSER_WIDTH	: integer	:= 0;
		C_M_AXI_RUSER_WIDTH	: integer	:= 0;
		C_M_AXI_BUSER_WIDTH	: integer	:= 0
	);
	port (

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
		s_axi_awvalid	: in std_logic;
		s_axi_awready	: out std_logic;
		s_axi_wdata	: in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		s_axi_wstrb	: in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
		s_axi_wlast	: in std_logic;
		s_axi_wvalid	: in std_logic;
		s_axi_wready	: out std_logic;
		s_axi_bid	: out std_logic_vector(C_S_AXI_ID_WIDTH-1 downto 0);
		s_axi_bresp	: out std_logic_vector(1 downto 0);
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
		s_axi_arvalid	: in std_logic;
		s_axi_arready	: out std_logic;
		s_axi_rid	: out std_logic_vector(C_S_AXI_ID_WIDTH-1 downto 0);
		s_axi_rdata	: out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		s_axi_rresp	: out std_logic_vector(1 downto 0);
		s_axi_rlast	: out std_logic;
		s_axi_rvalid	: out std_logic;
		s_axi_rready	: in std_logic;

		-- Ports of Axi Master Bus Interface M_AXI
		m_axi_aclk	: in std_logic;
		m_axi_aresetn	: in std_logic;
		m_axi_awid	: out std_logic_vector(C_M_AXI_ID_WIDTH-1 downto 0);
		m_axi_awaddr	: out std_logic_vector(C_M_AXI_ADDR_WIDTH-1 downto 0);
		m_axi_awlen	: out std_logic_vector(7 downto 0);
		m_axi_awsize	: out std_logic_vector(2 downto 0);
		m_axi_awburst	: out std_logic_vector(1 downto 0);
		m_axi_awlock	: out std_logic;
		m_axi_awcache	: out std_logic_vector(3 downto 0);
		m_axi_awprot	: out std_logic_vector(2 downto 0);
		m_axi_awqos	: out std_logic_vector(3 downto 0);
		m_axi_awvalid	: out std_logic;
		m_axi_awready	: in std_logic;
		m_axi_wdata	: out std_logic_vector(C_M_AXI_DATA_WIDTH-1 downto 0);
		m_axi_wstrb	: out std_logic_vector(C_M_AXI_DATA_WIDTH/8-1 downto 0);
		m_axi_wlast	: out std_logic;
		m_axi_wvalid	: out std_logic;
		m_axi_wready	: in std_logic;
		m_axi_bid	: in std_logic_vector(C_M_AXI_ID_WIDTH-1 downto 0);
		m_axi_bresp	: in std_logic_vector(1 downto 0);
		m_axi_bvalid	: in std_logic;
		m_axi_bready	: out std_logic;
		m_axi_arid	: out std_logic_vector(C_M_AXI_ID_WIDTH-1 downto 0);
		m_axi_araddr	: out std_logic_vector(C_M_AXI_ADDR_WIDTH-1 downto 0);
		m_axi_arlen	: out std_logic_vector(7 downto 0);
		m_axi_arsize	: out std_logic_vector(2 downto 0);
		m_axi_arburst	: out std_logic_vector(1 downto 0);
		m_axi_arlock	: out std_logic;
		m_axi_arcache	: out std_logic_vector(3 downto 0);
		m_axi_arprot	: out std_logic_vector(2 downto 0);
		m_axi_arqos	: out std_logic_vector(3 downto 0);
		m_axi_arvalid	: out std_logic;
		m_axi_arready	: in std_logic;
		m_axi_rid	: in std_logic_vector(C_M_AXI_ID_WIDTH-1 downto 0);
		m_axi_rdata	: in std_logic_vector(C_M_AXI_DATA_WIDTH-1 downto 0);
		m_axi_rresp	: in std_logic_vector(1 downto 0);
		m_axi_rlast	: in std_logic;
		m_axi_rvalid	: in std_logic;
		m_axi_rready	: out std_logic
	);
end ATL_v1_0;

architecture arch_imp of ATL_v1_0 is
    

    signal s_axi_trans_awaddr: std_logic_vector(31 downto 0) := (others => '0');
    signal s_axi_trans_araddr: std_logic_vector(31 downto 0) := (others => '0');
    
    begin
    
        s_axi_trans_awaddr <= s_axi_awaddr(31 downto 0);
        s_axi_trans_araddr <= s_axi_araddr(31 downto 0);
      
      
      -- Address Write Channel I/O Assignments
        m_axi_awid      <=  s_axi_awid;
        m_axi_awaddr    <=  s_axi_trans_awaddr;
        m_axi_awlen	    <=  s_axi_awlen;
        m_axi_awsize    <=  s_axi_awsize;
        m_axi_awburst   <=  s_axi_awburst;
        m_axi_awlock    <=  s_axi_awlock;
        m_axi_awcache   <=  s_axi_awcache;
        m_axi_awprot    <=  s_axi_awprot;
        m_axi_awqos     <=  s_axi_awqos;  
        m_axi_awvalid   <=  s_axi_awvalid; 
        s_axi_awready   <=  m_axi_awready;	 
        
        -- Write Data Channel I/O Assignments
        
        m_axi_wdata	    <=  s_axi_wdata;     
        m_axi_wstrb	    <=  s_axi_wstrb;	     
        m_axi_wlast	    <=  s_axi_wlast;	     
        m_axi_wvalid    <=  s_axi_wvalid;	
        s_axi_wready    <=  m_axi_wready;
             
        -- Write Response Channel I/O Assignments	 
             
        s_axi_bid	    <=  m_axi_bid;	     
        s_axi_bresp	    <=  m_axi_bresp;	     
        s_axi_bvalid    <=  m_axi_bvalid;	 
        m_axi_bready    <=  s_axi_bready;	 
        
        -- Address Read Channel I/O Assignments
        
        m_axi_arid      <=  s_axi_arid;	     
        m_axi_araddr    <=  s_axi_trans_araddr;	 
        m_axi_arlen	    <=  s_axi_arlen;	     
        m_axi_arsize    <=  s_axi_arsize; 
        m_axi_arburst   <=  s_axi_arburst;	 
        m_axi_arlock    <=  s_axi_arlock;	 
        m_axi_arcache   <=  s_axi_arcache;	 
        m_axi_arprot    <=  s_axi_arprot;	 
        m_axi_arqos	    <=  s_axi_arqos;
        m_axi_arvalid   <=  s_axi_arvalid;	 
        s_axi_arready   <=  m_axi_arready;	
        
        -- Read Data Channel I/O Assignments
        
        s_axi_rid       <=  m_axi_rid;    
        s_axi_rdata	    <=  m_axi_rdata;   
        s_axi_rresp	    <=  m_axi_rresp;    
        s_axi_rlast	    <=  m_axi_rlast;    
        s_axi_rvalid    <=  m_axi_rvalid;	 
        m_axi_rready    <=  s_axi_rready;
        
       

end arch_imp;
