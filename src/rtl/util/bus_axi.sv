// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

`include "prim_assert.sv"

module bus_axi #(
  parameter int NrDevices    = 1,
  parameter int NrHosts      = 1,
  parameter int DataWidth    = 32,
  parameter int AddressWidth = 32
) (
  input                           clk_i,
  input                           rst_ni,

  // Hosts (masters)
  //input                           host_req_i    [NrHosts], // data_req_o - AWVALID, ARVALID, RREADY
  input                           m_awvalid   [NrHosts],
  input                           m_arvalid   [NrHosts],
  input                           m_rready    [NrHosts],
  //output logic                    host_gnt_o    [NrHosts], // data_gnt_i - AWREADY, WREADY
  output                          m_awready   [NrHosts],
  output                          m_wready    [NrHosts],

  //input        [AddressWidth-1:0] host_addr_i   [NrHosts], // data_addr_o - AWADDR, ARADDR
  input        [AddressWidth-1:0] m_awaddr    [NrHosts],
  input        [AddressWidth-1:0] m_araddr    [NrHosts],

  //input                           host_we_i     [NrHosts], // write en (store)
  input        [ DataWidth/8-1:0] m_wstrb     [NrHosts], // WSTRB
  input        [   DataWidth-1:0] m_wdata     [NrHosts], // write data (store) - WDATA

  input                           m_wvalid    [NrHosts],
  output logic                    m_arready   [NrHosts],

  //output logic                    host_rvalid_o [NrHosts], // RVALID, BVALID
  output logic                    m_rvalid    [NrHosts],
  output logic                    m_bvalid    [NrHosts],
  input                           m_bready    [NrHosts],

  output logic [   DataWidth-1:0] m_rdata     [NrHosts], // RDATA
  output                   [1:0]  m_bresp     [NrHosts],
  output                   [1:0]  m_rresp     [NrHosts],

  input                    [2:0]  m_arprot    [NrHosts],
  input                    [2:0]  m_awprot    [NrHosts],



  // Devices (slaves)
  output logic                           s_awvalid   [NrDevices],
  output logic                           s_arvalid   [NrDevices],
  output logic                           s_rready    [NrDevices],

  input                                  s_awready   [NrDevices],
  input                                  s_wready    [NrDevices],
  output logic        [AddressWidth-1:0] s_awaddr    [NrDevices],
  output logic        [AddressWidth-1:0] s_araddr    [NrDevices],

  output logic        [ DataWidth/8-1:0] s_wstrb     [NrDevices],
  output logic        [   DataWidth-1:0] s_wdata     [NrDevices], 
  output                                 s_wvalid    [NrDevices],
  input                                  s_arready   [NrDevices],

  input                                  s_rvalid    [NrDevices],
  input                                  s_bvalid    [NrDevices],
  output logic                           s_bready    [NrDevices],

  input               [   DataWidth-1:0] s_rdata     [NrDevices],

  input                          [1:0]   s_bresp     [NrDevices],
  input                          [1:0]   s_rresp     [NrDevices],

  output                         [2:0]   s_arprot    [NrDevices],
  output                         [2:0]   s_awprot    [NrDevices],
  

  // Device address map
  input        [AddressWidth-1:0] cfg_device_addr_base [NrDevices],
  input        [AddressWidth-1:0] cfg_device_addr_mask [NrDevices]
);

  localparam int unsigned NumBitsHostSel = NrHosts > 1 ? $clog2(NrHosts) : 1;
  localparam int unsigned NumBitsDeviceSel = NrDevices > 1 ? $clog2(NrDevices) : 1;

  logic [NumBitsHostSel-1:0] host_sel_req, host_sel_resp;
  logic [NumBitsDeviceSel-1:0] device_sel_req, device_sel_resp;

  // Master select prio arbiter
  always_comb begin
    host_sel_req = '0;
    for (integer host = NrHosts - 1; host >= 0; host = host - 1) begin
      if (m_arvalid[host] || m_awvalid[host]) begin
        host_sel_req = NumBitsHostSel'(host);
      end
    end
  end

  // Device select
  always_comb begin
    device_sel_req = '0;
    for (integer device = 0; device < NrDevices; device = device + 1) begin
      if (m_arvalid[host_sel_req]) begin
        if ((m_araddr[host_sel_req] & cfg_device_addr_mask[device])
            == cfg_device_addr_base[device]) begin
          device_sel_req = NumBitsDeviceSel'(device);
        end
      end
      else if (m_awvalid[host_sel_req]) begin
        if ((m_awaddr[host_sel_req] & cfg_device_addr_mask[device])
            == cfg_device_addr_base[device]) begin
          device_sel_req = NumBitsDeviceSel'(device);
        end
      end
    end
  end

  always_ff @(posedge clk_i or negedge rst_ni) begin
     if (!rst_ni) begin
        host_sel_resp <= '0;
        device_sel_resp <= '0;
     end else begin
        // Responses are always expected 1 cycle after the request
        device_sel_resp <= device_sel_req;
        host_sel_resp <= host_sel_req;
     end
  end

  always_comb begin
    for (integer device = 0; device < NrDevices; device = device + 1) begin
      if (NumBitsDeviceSel'(device) == device_sel_req) begin
        s_awvalid[device] = m_awvalid[host_sel_req];
        s_arvalid[device] = m_arvalid[host_sel_req];
        s_rready[device]  = m_rready[host_sel_req];
        s_wvalid[device]  = m_wvalid[host_sel_req];
        s_wstrb[device]   = m_wstrb[host_sel_req];
        s_wdata[device]   = m_wdata[host_sel_req];
        s_awaddr[device]  = m_awaddr[host_sel_req];
        s_bready[device]  = m_bready[host_sel_req];
        s_araddr[device]  = m_araddr[host_sel_req];
        s_bready[device]  = m_bready[host_sel_req];
        s_arprot[device]  = m_arprot[host_sel_req];
        s_awprot[device]  = m_arprot[host_sel_req];
      end else begin 
        s_awvalid[device] = 'b0;
        s_arvalid[device] = 'b0;
        s_rready[device]  = 'b0;
        s_wvalid[device]  = 'b0;
        s_awvalid[device] = 'b0;
        s_wstrb[device]   = 'b0;
        s_wdata[device]   = 'b0;
        s_awaddr[device]  = 'b0;
        s_bready[device]  = 'b0;
        s_araddr[device]  = 'b0;
        s_bready[device]  = 'b0;
        s_arprot[device]  = 'b0;
        s_awprot[device]  = 'b0;
      end
    end
  end

  always_comb begin
    for (integer host = 0; host < NrHosts; host = host + 1) begin
      if (NumBitsHostSel'(host) == host_sel_resp) begin
        m_rvalid[host]  = s_rvalid[device_sel_resp];
        m_rdata[host]   = s_rdata[device_sel_resp];
        m_arready[host] = s_arready[device_sel_resp];
        m_awready[host] = s_awready[device_sel_resp];
        m_wready[host]  = s_wready[device_sel_resp];
        m_bvalid[host]  = s_bvalid[device_sel_resp];
        m_bresp[host]   = s_bresp[device_sel_resp];
        m_rresp[host]   = s_rresp[device_sel_resp];
      end else begin
        m_rvalid[host]  = 'b0;
        m_rdata[host]   = 'b0;
        m_arready[host] = 'b0;
        m_awready[host] = 'b0;
        m_wready[host]  = 'b0;
        m_bvalid[host]  = 'b0;
        m_bresp[host]   = 'b0;
        m_rresp[host]   = 'b0;
      end
    end
  end
endmodule
