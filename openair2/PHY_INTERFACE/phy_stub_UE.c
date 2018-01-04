
//#include "openair1/PHY/defs.h"
//#include "openair2/PHY_INTERFACE/IF_Module.h"
//#include "openair1/PHY/extern.h"
#include "openair2/LAYER2/MAC/extern.h"
#include "openair2/LAYER2/MAC/defs.h"
#include "openair2/LAYER2/MAC/proto.h"
//#include "openair2/LAYER2/MAC/vars.h"
#include "openair1/SCHED/defs.h"
//#include "common/ran_context.h"
#include "openair2/PHY_INTERFACE/phy_stub_UE.h"

//#define DEADLINE_SCHEDULER 1

//#include "nfapi_pnf_interface.h"
//#include "nfapi.h"
//#include "nfapi_pnf.h"




//extern uint8_t nfapi_pnf;
//UL_IND_t *UL_INFO;
extern nfapi_tx_request_pdu_t* tx_request_pdu[1023][10][10];

void Msg1_transmitted(module_id_t module_idP,uint8_t CC_id,frame_t frameP, uint8_t eNB_id);
void Msg3_transmitted(module_id_t module_idP,uint8_t CC_id,frame_t frameP, uint8_t eNB_id);



void fill_rx_indication_UE_MAC(module_id_t Mod_id,int frame,int subframe, UL_IND_t* UL_INFO, uint8_t *ulsch_buffer, uint16_t buflen, uint16_t rnti)
{
	  nfapi_rx_indication_pdu_t *pdu;

	  int timing_advance_update;
	  //int sync_pos;

	  //uint32_t harq_pid = subframe2harq_pid(&eNB->frame_parms,
	 //					frame,subframe);

	  //UL_IND_t *UL_INFO = (UL_IND_t*)malloc16(sizeof(UL_IND_t));


	  pthread_mutex_lock(&UE_mac_inst[Mod_id].UL_INFO_mutex);

	  //eNB->UL_INFO.rx_ind.sfn_sf                    = frame<<4| subframe;
	  //eNB->UL_INFO.rx_ind.rx_indication_body.tl.tag = NFAPI_RX_INDICATION_BODY_TAG;

	  pdu                                    = &UL_INFO->rx_ind.rx_indication_body.rx_pdu_list[UL_INFO->rx_ind.rx_indication_body.number_of_pdus];

	  //  pdu->rx_ue_information.handle          = eNB->ulsch[UE_id]->handle;
	  pdu->rx_ue_information.tl.tag          = NFAPI_RX_UE_INFORMATION_TAG;
	  pdu->rx_ue_information.rnti            = rnti;
	  pdu->rx_indication_rel8.tl.tag         = NFAPI_RX_INDICATION_REL8_TAG;
	  //pdu->rx_indication_rel8.length         = eNB->ulsch[UE_id]->harq_processes[harq_pid]->TBS>>3;
	  pdu->rx_indication_rel8.length         = buflen;
	  pdu->rx_indication_rel8.offset         = 1;   // DJP - I dont understand - but broken unless 1 ????  0;  // filled in at the end of the UL_INFO formation
	  pdu->data                              = ulsch_buffer;
	  // estimate timing advance for MAC
	  //sync_pos                               = lte_est_timing_advance_pusch(eNB,UE_id);
	  timing_advance_update                  = 0;  //Panos: Don't know what to put here
	  pdu->rx_indication_rel8.timing_advance = timing_advance_update;

	  //  if (timing_advance_update > 10) { dump_ulsch(eNB,frame,subframe,UE_id); exit(-1);}
	  //  if (timing_advance_update < -10) { dump_ulsch(eNB,frame,subframe,UE_id); exit(-1);}*/
	  /*switch (eNB->frame_parms.N_RB_DL) {
	  case 6:
	    pdu->rx_indication_rel8.timing_advance = timing_advance_update;
	    break;
	  case 15:
	    pdu->rx_indication_rel8.timing_advance = timing_advance_update/2;
	    break;
	  case 25:
	    pdu->rx_indication_rel8.timing_advance = timing_advance_update/4;
	    break;
	  case 50:
	    pdu->rx_indication_rel8.timing_advance = timing_advance_update/8;
	    break;
	  case 75:
	    pdu->rx_indication_rel8.timing_advance = timing_advance_update/12;
	    break;
	  case 100:
	    pdu->rx_indication_rel8.timing_advance = timing_advance_update/16;
	    break;
	  }
	  // put timing advance command in 0..63 range
	  timing_advance_update += 31;
	  if (timing_advance_update < 0)  timing_advance_update = 0;
	  if (timing_advance_update > 63) timing_advance_update = 63;
	  pdu->rx_indication_rel8.timing_advance = timing_advance_update;*/

	  // estimate UL_CQI for MAC (from antenna port 0 only)

	  // Panos dependency from eNB not sure how to substitute this. Should we hardcode it?
	  //int SNRtimes10 = dB_fixed_times10(eNB->pusch_vars[UE_id]->ulsch_power[0]) - 200;//(10*eNB->measurements.n0_power_dB[0]);
	  int SNRtimes10 = 640;

	  if      (SNRtimes10 < -640) pdu->rx_indication_rel8.ul_cqi=0;
	  else if (SNRtimes10 >  635) pdu->rx_indication_rel8.ul_cqi=255;
	  else                        pdu->rx_indication_rel8.ul_cqi=(640+SNRtimes10)/5;


	  /*LOG_D(PHY,"[PUSCH %d] Filling RX_indication with SNR %d (%d), timing_advance %d (update %d)\n",
		harq_pid,SNRtimes10,pdu->rx_indication_rel8.ul_cqi,pdu->rx_indication_rel8.timing_advance,
		timing_advance_update);*/

	  UL_INFO->rx_ind.rx_indication_body.number_of_pdus++;
	  pthread_mutex_unlock(&UE_mac_inst[Mod_id].UL_INFO_mutex);


}

void fill_sr_indication_UE_MAC(int Mod_id,int frame,int subframe, UL_IND_t *UL_INFO, uint16_t rnti) {

  pthread_mutex_lock(&UE_mac_inst[Mod_id].UL_INFO_mutex);
  nfapi_sr_indication_pdu_t *pdu =   &UL_INFO->sr_ind.sr_indication_body.sr_pdu_list[UL_INFO->rx_ind.rx_indication_body.number_of_pdus];

  pdu->instance_length                                = 0; // don't know what to do with this
  //  pdu->rx_ue_information.handle                       = handle;
  pdu->rx_ue_information.tl.tag                       = NFAPI_RX_UE_INFORMATION_TAG;
  pdu->rx_ue_information.rnti                         = rnti; //UE_mac_inst[Mod_id].crnti;; //Panos: Is this the right RNTI?


  // Panos dependency from PHY not sure how to substitute this. Should we hardcode it?
  //int SNRtimes10 = dB_fixed_times10(stat) - 200;//(10*eNB->measurements.n0_power_dB[0]);
  int SNRtimes10 = 640;


  if      (SNRtimes10 < -640) pdu->ul_cqi_information.ul_cqi=0;
  else if (SNRtimes10 >  635) pdu->ul_cqi_information.ul_cqi=255;
  else                        pdu->ul_cqi_information.ul_cqi=(640+SNRtimes10)/5;
  pdu->ul_cqi_information.channel = 0;

  UL_INFO->rx_ind.rx_indication_body.number_of_pdus++;
  pthread_mutex_unlock(&UE_mac_inst[Mod_id].UL_INFO_mutex);
}


void fill_crc_indication_UE_MAC(int Mod_id,int frame,int subframe, UL_IND_t *UL_INFO, uint8_t crc_flag) {

  pthread_mutex_lock(&UE_mac_inst[Mod_id].UL_INFO_mutex);
  nfapi_crc_indication_pdu_t *pdu =   &UL_INFO->crc_ind.crc_indication_body.crc_pdu_list[UL_INFO->crc_ind.crc_indication_body.number_of_crcs];

  //eNB->UL_INFO.crc_ind.sfn_sf                         = frame<<4 | subframe;
  //eNB->UL_INFO.crc_ind.crc_indication_body.tl.tag     = NFAPI_CRC_INDICATION_BODY_TAG;

  pdu->instance_length                                = 0; // don't know what to do with this
  //  pdu->rx_ue_information.handle                       = handle;
  pdu->rx_ue_information.tl.tag                       = NFAPI_RX_UE_INFORMATION_TAG;
  pdu->rx_ue_information.rnti                         = UE_mac_inst[Mod_id].crnti;
  pdu->crc_indication_rel8.tl.tag                     = NFAPI_CRC_INDICATION_REL8_TAG;
  pdu->crc_indication_rel8.crc_flag                   = crc_flag;

  UL_INFO->crc_ind.crc_indication_body.number_of_crcs++;

  LOG_D(PHY, "%s() rnti:%04x pdus:%d\n", __FUNCTION__, pdu->rx_ue_information.rnti, UL_INFO->crc_ind.crc_indication_body.number_of_crcs);

  pthread_mutex_unlock(&UE_mac_inst[Mod_id].UL_INFO_mutex);
}

void fill_rach_indication_UE_MAC(int Mod_id,int frame,int subframe, UL_IND_t *UL_INFO, uint8_t ra_PreambleIndex, uint16_t ra_RNTI) {

	//LOG_I(MAC, "Panos-D: fill_rach_indication_UE_MAC 1 \n");
	pthread_mutex_lock(&UE_mac_inst[Mod_id].UL_INFO_mutex);
	UL_INFO = (UL_IND_t*)malloc(sizeof(UL_IND_t));



	//LOG_I(MAC, "Panos-D: fill_rach_indication_UE_MAC 2 \n");
	    UL_INFO->rach_ind.rach_indication_body.number_of_preambles                 = 1;

	    //eNB->UL_INFO.rach_ind.preamble_list                       = &eNB->preamble_list[0];
	    UL_INFO->rach_ind.header.message_id                         = NFAPI_RACH_INDICATION;
	    UL_INFO->rach_ind.sfn_sf                                    = frame<<4 | subframe;

	    UL_INFO->rach_ind.rach_indication_body.tl.tag                              = NFAPI_RACH_INDICATION_BODY_TAG;


	    UL_INFO->rach_ind.rach_indication_body.preamble_list = (nfapi_preamble_pdu_t*)malloc(UL_INFO->rach_ind.rach_indication_body.number_of_preambles*sizeof(nfapi_preamble_pdu_t));
	    UL_INFO->rach_ind.rach_indication_body.preamble_list[0].preamble_rel8.tl.tag   		= NFAPI_PREAMBLE_REL8_TAG;
	    UL_INFO->rach_ind.rach_indication_body.preamble_list[0].preamble_rel8.timing_advance = 0; //Panos: Not sure about that

	    //Panos: The two following should get extracted from the call to get_prach_resources().
	    UL_INFO->rach_ind.rach_indication_body.preamble_list[0].preamble_rel8.preamble = ra_PreambleIndex;
	    UL_INFO->rach_ind.rach_indication_body.preamble_list[0].preamble_rel8.rnti 	  = ra_RNTI;
	    //UL_INFO->rach_ind.rach_indication_body.number_of_preambles++;


	    UL_INFO->rach_ind.rach_indication_body.preamble_list[0].preamble_rel13.rach_resource_type = 0;
	    UL_INFO->rach_ind.rach_indication_body.preamble_list[0].instance_length					 = 0;


	        // If NFAPI PNF then we need to send the message to the VNF
	        //if (nfapi_pnf == 1)
	        //{
	        //Panos: Not sure if we need the following. They refer to nfapi_rach_indication_t type
	        //so we cannot insert it to the UL_INFO which has an nfapi_rach_indication_body_t type.
	    	//Probably it should be part of UL_indication() function before calling oai_nfapi_rach_ind(&rach_ind).

	    	  /*nfapi_rach_indication_t *rach_ind;
	          rach_ind->header.message_id = NFAPI_RACH_INDICATION;
	          rach_ind->sfn_sf = frame<<4 | subframe;
	          rach_ind->rach_indication_body = UL_INFO->rach_ind;*/

	          LOG_E(PHY,"\n\n\n\nDJP - this needs to be sent to VNF **********************************************\n\n\n\n");
	          LOG_E(PHY,"UE Filling NFAPI indication for RACH : TA %d, Preamble %d, rnti %x, rach_resource_type %d\n",
	        	  UL_INFO->rach_ind.rach_indication_body.preamble_list[0].preamble_rel8.timing_advance,
	        	  UL_INFO->rach_ind.rach_indication_body.preamble_list[0].preamble_rel8.preamble,
	        	  UL_INFO->rach_ind.rach_indication_body.preamble_list[0].preamble_rel8.rnti,
	        	  UL_INFO->rach_ind.rach_indication_body.preamble_list[0].preamble_rel13.rach_resource_type);

	          //Panos: This function is currently defined only in the nfapi-RU-RAU-split so we should call it when we merge
	          // with that branch.
	          oai_nfapi_rach_ind(&UL_INFO->rach_ind);
	          free(UL_INFO->rach_ind.rach_indication_body.preamble_list);
	          free(UL_INFO);

	        //}
	      pthread_mutex_unlock(&UE_mac_inst[Mod_id].UL_INFO_mutex);

}

void fill_ulsch_cqi_indication_UE_MAC(int Mod_id, uint16_t frame,uint8_t subframe, UL_IND_t *UL_INFO, uint16_t rnti) {
	pthread_mutex_lock(&UE_mac_inst[Mod_id].UL_INFO_mutex);
	nfapi_cqi_indication_pdu_t *pdu         = &UL_INFO->cqi_ind.cqi_pdu_list[UL_INFO->cqi_ind.number_of_cqis];
	nfapi_cqi_indication_raw_pdu_t *raw_pdu = &UL_INFO->cqi_ind.cqi_raw_pdu_list[UL_INFO->cqi_ind.number_of_cqis];

	pdu->rx_ue_information.rnti = rnti;
	//if (ulsch_harq->cqi_crc_status != 1)
	//Panos: Since we assume that CRC flag is always 0 (ACK) I guess that data_offset should always be 0.
	pdu->cqi_indication_rel9.data_offset = 0;
	//else               pdu->cqi_indication_rel9.data_offset = 1; // fill in after all cqi_indications have been generated when non-zero

	// by default set O to rank 1 value
	//pdu->cqi_indication_rel9.length = (ulsch_harq->Or1>>3) + ((ulsch_harq->Or1&7) > 0 ? 1 : 0);
	// Panos: Not useful field for our case
	pdu->cqi_indication_rel9.length = 0;
	pdu->cqi_indication_rel9.ri[0]  = 0;

  // if we have RI bits, set them and if rank2 overwrite O
  /*if (ulsch_harq->O_RI>0) {
    pdu->cqi_indication_rel9.ri[0] = ulsch_harq->o_RI[0];
    if (ulsch_harq->o_RI[0] == 2)   pdu->cqi_indication_rel9.length = (ulsch_harq->Or2>>3) + ((ulsch_harq->Or2&7) > 0 ? 1 : 0);
    pdu->cqi_indication_rel9.timing_advance = 0;
  }*/

	pdu->cqi_indication_rel9.timing_advance = 0;
  pdu->cqi_indication_rel9.number_of_cc_reported = 1;
  pdu->ul_cqi_information.channel = 1; // PUSCH

  //Panos: Not sure how to substitute this. This should be the actual CQI value? So can
  // we hardcode it to a specific value?
  //memcpy((void*)raw_pdu->pdu,ulsch_harq->o,pdu->cqi_indication_rel9.length);
  raw_pdu->pdu[0] = 7;



  UL_INFO->cqi_ind.number_of_cqis++;
  pthread_mutex_unlock(&UE_mac_inst[Mod_id].UL_INFO_mutex);

}

void fill_ulsch_harq_indication_UE_MAC(int Mod_id, int frame,int subframe, UL_IND_t *UL_INFO, nfapi_ul_config_ulsch_harq_information *harq_information, uint16_t rnti)
{

  //int UE_id = find_dlsch(rnti,eNB,SEARCH_EXIST);
  //AssertFatal(UE_id>=0,"UE_id doesn't exist\n");

  pthread_mutex_lock(&UE_mac_inst[Mod_id].UL_INFO_mutex);
  nfapi_harq_indication_pdu_t *pdu =   &UL_INFO->harq_ind.harq_indication_body.harq_pdu_list[UL_INFO->harq_ind.harq_indication_body.number_of_harqs];
  int i;

  pdu->instance_length                                = 0; // don't know what to do with this
  //  pdu->rx_ue_information.handle                       = handle;
  pdu->rx_ue_information.rnti                         = rnti;

  //Panos: For now we consider only FDD
  //if (eNB->frame_parms.frame_type == FDD) {
    pdu->harq_indication_fdd_rel13.mode = 0;
    pdu->harq_indication_fdd_rel13.number_of_ack_nack = harq_information->harq_information_rel10.harq_size;

    //Panos: Could this be wrong? Is the number_of_ack_nack field equivalent to O_ACK?
    //pdu->harq_indication_fdd_rel13.number_of_ack_nack = ulsch_harq->O_ACK;

    for (i=0;i<harq_information->harq_information_rel10.harq_size;i++) {
      //AssertFatal(ulsch_harq->o_ACK[i] == 0 || ulsch_harq->o_ACK[i] == 1, "harq_ack[%d] is %d, should be 1,2 or 4\n",i,ulsch_harq->o_ACK[i]);

      pdu->harq_indication_fdd_rel13.harq_tb_n[i] = 1; //Panos: Assuming always an ACK (No NACK or DTX)
      // release DLSCH if needed
      //if (ulsch_harq->o_ACK[i] == 1) release_harq(eNB,UE_id,i,frame,subframe,0xffff);

    }
  //}
  /*else { // TDD
    M=ul_ACK_subframe2_M(&eNB->frame_parms,
			 subframe);

    pdu->harq_indication_fdd_rel13.mode = 1-bundling;
    pdu->harq_indication_fdd_rel13.number_of_ack_nack = ulsch_harq->O_ACK;

    for (i=0;i<ulsch_harq->O_ACK;i++) {
      AssertFatal(ulsch_harq->o_ACK[i] == 0 || ulsch_harq->o_ACK[i] == 1, "harq_ack[%d] is %d, should be 1,2 or 4\n",i,ulsch_harq->o_ACK[i]);

      pdu->harq_indication_tdd_rel13.harq_data[0].multiplex.value_0 = 2-ulsch_harq->o_ACK[i];
      // release DLSCH if needed
      if (ulsch_harq->o_ACK[i] == 1) release_harq(eNB,UE_id,i,frame,subframe,0xffff);
      if      (M==1 && ulsch_harq->O_ACK==1 && ulsch_harq->o_ACK[i] == 1) release_harq(eNB,UE_id,0,frame,subframe,0xffff);
      else if (M==1 && ulsch_harq->O_ACK==2 && ulsch_harq->o_ACK[i] == 1) release_harq(eNB,UE_id,i,frame,subframe,0xffff);
      else if (M>1 && ulsch_harq->o_ACK[i] == 1) {
	// spatial bundling
	release_harq(eNB,UE_id,0,frame,subframe,1<<i);
	release_harq(eNB,UE_id,1,frame,subframe,1<<i);
      }
    }
  }*/

  UL_INFO->harq_ind.harq_indication_body.number_of_harqs++;
  pthread_mutex_unlock(&UE_mac_inst[Mod_id].UL_INFO_mutex);
}


void fill_uci_harq_indication_UE_MAC(int Mod_id,
			      int frame,
			      int subframe,
			      UL_IND_t *UL_INFO,
			      nfapi_ul_config_harq_information *harq_information,
			      uint16_t rnti
			      /*uint8_t tdd_mapping_mode,
			      uint16_t tdd_multiplexing_mask*/) {

  //int UE_id=find_dlsch(uci->rnti,eNB,SEARCH_EXIST);
  //AssertFatal(UE_id>=0,"UE_id doesn't exist\n");


  pthread_mutex_lock(&UE_mac_inst[Mod_id].UL_INFO_mutex);
  nfapi_harq_indication_pdu_t *pdu =   &UL_INFO->harq_ind.harq_indication_body.harq_pdu_list[UL_INFO->harq_ind.harq_indication_body.number_of_harqs];

  pdu->instance_length                                = 0; // don't know what to do with this
  //  pdu->rx_ue_information.handle                       = handle;
  pdu->rx_ue_information.rnti                         = rnti;

  // estimate UL_CQI for MAC (from antenna port 0 only)

  // Panos: Set static SNR for now
  //int SNRtimes10 = dB_fixed_times10(uci->stat) - 200;//(10*eNB->measurements.n0_power_dB[0]);
  int SNRtimes10 = 640;

  //if (SNRtimes10 < -100) LOG_I(PHY,"uci->stat %d \n",uci->stat);

  if      (SNRtimes10 < -640) pdu->ul_cqi_information.ul_cqi=0;
  else if (SNRtimes10 >  635) pdu->ul_cqi_information.ul_cqi=255;
  else                        pdu->ul_cqi_information.ul_cqi=(640+SNRtimes10)/5;
  pdu->ul_cqi_information.channel = 0;

  //Panos: Considering only FDD for now
  //if (eNB->frame_parms.frame_type == FDD) {

    //Panos: Condition taken from fapi_l1::handle_uci_harq_information() function
    if ((harq_information->harq_information_rel9_fdd.ack_nack_mode == 0) &&
          (harq_information->harq_information_rel9_fdd.harq_size == 1)) {
    //if (uci->pucch_fmt == pucch_format1a) {
      pdu->harq_indication_fdd_rel13.mode = 0;
      pdu->harq_indication_fdd_rel13.number_of_ack_nack = 1;

      //AssertFatal(harq_ack[0] == 1 || harq_ack[0] == 2 || harq_ack[0] == 4, "harq_ack[0] is %d, should be 1,2 or 4\n",harq_ack[0]);
      pdu->harq_indication_fdd_rel13.harq_tb_n[0] = 1; //Panos: Assuming always an ACK (No NACK or DTX)


    }
    else if ((harq_information->harq_information_rel9_fdd.ack_nack_mode == 0) &&
                 (harq_information->harq_information_rel9_fdd.harq_size == 2)) {
      pdu->harq_indication_fdd_rel13.mode = 0;
      pdu->harq_indication_fdd_rel13.number_of_ack_nack = 2;
      //AssertFatal(harq_ack[0] == 1 || harq_ack[0] == 2 || harq_ack[1] == 4, "harq_ack[0] is %d, should be 0,1 or 4\n",harq_ack[0]);
      //AssertFatal(harq_ack[1] == 1 || harq_ack[1] == 2 || harq_ack[1] == 4, "harq_ack[1] is %d, should be 0,1 or 4\n",harq_ack[1]);
      pdu->harq_indication_fdd_rel13.harq_tb_n[0] = 1; //Panos: Assuming always an ACK (No NACK or DTX)
      pdu->harq_indication_fdd_rel13.harq_tb_n[1] = 1; //Panos: Assuming always an ACK (No NACK or DTX)
      // release DLSCH if needed
      //if (harq_ack[0] == 1) release_harq(eNB,UE_id,0,frame,subframe,0xffff);
      //if (harq_ack[1] == 1) release_harq(eNB,UE_id,1,frame,subframe,0xffff);
    }
    else AssertFatal(1==0,"only format 1a/b for now, received \n");
  //}
  /*else { // TDD

    AssertFatal(tdd_mapping_mode==0 || tdd_mapping_mode==1 || tdd_mapping_mode==2,
		"Illegal tdd_mapping_mode %d\n",tdd_mapping_mode);

    pdu->harq_indication_tdd_rel13.mode = tdd_mapping_mode;

    switch (tdd_mapping_mode) {
    case 0: // bundling

      if (uci->pucch_fmt == pucch_format1a) {
	pdu->harq_indication_tdd_rel13.number_of_ack_nack = 1;
	AssertFatal(harq_ack[0] == 1 || harq_ack[0] == 2 || harq_ack[0] == 4, "harq_ack[0] is %d, should be 1,2 or 4\n",harq_ack[0]);
	pdu->harq_indication_tdd_rel13.harq_data[0].bundling.value_0 = harq_ack[0];
	// release all bundled DLSCH if needed
	if (harq_ack[0] == 1) release_harq(eNB,UE_id,0,frame,subframe,0xffff);
      }
      else if (uci->pucch_fmt == pucch_format1b) {
	pdu->harq_indication_tdd_rel13.number_of_ack_nack = 2;
	AssertFatal(harq_ack[0] == 1 || harq_ack[0] == 2 || harq_ack[1] == 4, "harq_ack[0] is %d, should be 0,1 or 4\n",harq_ack[0]);
	AssertFatal(harq_ack[1] == 1 || harq_ack[1] == 2 || harq_ack[1] == 4, "harq_ack[1] is %d, should be 0,1 or 4\n",harq_ack[1]);
	pdu->harq_indication_tdd_rel13.harq_data[0].bundling.value_0 = harq_ack[0];
	pdu->harq_indication_tdd_rel13.harq_data[1].bundling.value_0 = harq_ack[1];
	// release all DLSCH if needed
	if (harq_ack[0] == 1) release_harq(eNB,UE_id,0,frame,subframe,0xffff);
	if (harq_ack[1] == 1) release_harq(eNB,UE_id,1,frame,subframe,0xffff);
      }
      break;
    case 1: // multiplexing
      AssertFatal(uci->pucch_fmt == pucch_format1b,"uci->pucch_format %d is not format1b\n",uci->pucch_fmt);

      if (uci->num_pucch_resources == 1 && uci->pucch_fmt == pucch_format1a) {
	pdu->harq_indication_tdd_rel13.number_of_ack_nack = 1;
	AssertFatal(harq_ack[0] == 1 || harq_ack[0] == 2 || harq_ack[0] == 4, "harq_ack[0] is %d, should be 1,2 or 4\n",harq_ack[0]);
	pdu->harq_indication_tdd_rel13.harq_data[0].multiplex.value_0 = harq_ack[0];
	// release all DLSCH if needed
	if (harq_ack[0] == 1) release_harq(eNB,UE_id,0,frame,subframe,0xffff);
      }
      else if (uci->num_pucch_resources == 1 && uci->pucch_fmt == pucch_format1b) {
	pdu->harq_indication_tdd_rel13.number_of_ack_nack = 2;
	AssertFatal(harq_ack[0] == 1 || harq_ack[0] == 2 || harq_ack[1] == 4, "harq_ack[0] is %d, should be 0,1 or 4\n",harq_ack[0]);
	AssertFatal(harq_ack[1] == 1 || harq_ack[1] == 2 || harq_ack[1] == 4, "harq_ack[1] is %d, should be 0,1 or 4\n",harq_ack[1]);
	pdu->harq_indication_tdd_rel13.harq_data[0].multiplex.value_0 = harq_ack[0];
	pdu->harq_indication_tdd_rel13.harq_data[1].multiplex.value_0 = harq_ack[1];
	// release all DLSCH if needed
	if (harq_ack[0] == 1) release_harq(eNB,UE_id,0,frame,subframe,0xffff);
	if (harq_ack[1] == 1) release_harq(eNB,UE_id,1,frame,subframe,0xffff);
      }
      else { // num_pucch_resources (M) > 1
	pdu->harq_indication_tdd_rel13.number_of_ack_nack = uci->num_pucch_resources;

	pdu->harq_indication_tdd_rel13.harq_data[0].multiplex.value_0 = harq_ack[0];
	pdu->harq_indication_tdd_rel13.harq_data[1].multiplex.value_0 = harq_ack[1];
	if (uci->num_pucch_resources == 3) 	pdu->harq_indication_tdd_rel13.harq_data[2].multiplex.value_0 = harq_ack[2];
	if (uci->num_pucch_resources == 4) 	pdu->harq_indication_tdd_rel13.harq_data[3].multiplex.value_0 = harq_ack[3];
	// spatial-bundling in this case so release both HARQ if necessary
	release_harq(eNB,UE_id,0,frame,subframe,tdd_multiplexing_mask);
	release_harq(eNB,UE_id,1,frame,subframe,tdd_multiplexing_mask);
      }
      break;
    case 2: // special bundling (SR collision)
      pdu->harq_indication_tdd_rel13.number_of_ack_nack = 1;
      int tdd_config5_sf2scheds=0;
      if (eNB->frame_parms.tdd_config==5) tdd_config5_sf2scheds = getM(eNB,frame,subframe);

      switch (harq_ack[0]) {
      case 0:
	break;
      case 1: // check if M=1,4,7
	if (uci->num_pucch_resources == 1 || uci->num_pucch_resources == 4 ||
	    tdd_config5_sf2scheds == 1 || tdd_config5_sf2scheds == 4 || tdd_config5_sf2scheds == 7) {
	  release_harq(eNB,UE_id,0,frame,subframe,0xffff);
	  release_harq(eNB,UE_id,1,frame,subframe,0xffff);
	}
	break;
      case 2: // check if M=2,5,8
	if (uci->num_pucch_resources == 2 || tdd_config5_sf2scheds == 2 ||
	    tdd_config5_sf2scheds == 5 || tdd_config5_sf2scheds == 8) {
	  release_harq(eNB,UE_id,0,frame,subframe,0xffff);
	  release_harq(eNB,UE_id,1,frame,subframe,0xffff);
	}
	break;
      case 3: // check if M=3,6,9
	if (uci->num_pucch_resources == 3 || tdd_config5_sf2scheds == 3 ||
	    tdd_config5_sf2scheds == 6 || tdd_config5_sf2scheds == 9) {
	  release_harq(eNB,UE_id,0,frame,subframe,0xffff);
	  release_harq(eNB,UE_id,1,frame,subframe,0xffff);
	}
	break;
      }
      break;

    }
  } //TDD*/


  UL_INFO->harq_ind.harq_indication_body.number_of_harqs++;
  LOG_E(PHY,"Incremented eNB->UL_INFO.harq_ind.number_of_harqs:%d\n", UL_INFO->harq_ind.harq_indication_body.number_of_harqs);
  pthread_mutex_unlock(&UE_mac_inst[Mod_id].UL_INFO_mutex);

}


void handle_nfapi_ul_pdu_UE_MAC(module_id_t Mod_id,
                         nfapi_ul_config_request_pdu_t *ul_config_pdu,
                         uint16_t frame,uint8_t subframe,uint8_t srs_present)
{
  nfapi_ul_config_ulsch_pdu_rel8_t *rel8 = &ul_config_pdu->ulsch_pdu.ulsch_pdu_rel8;
  LOG_I(MAC, "Panos-D: handle_nfapi_ul_pdu_UE_MAC 1 \n");

  //int8_t UE_id;

  // check if we have received a dci for this ue and ulsch descriptor is configured

  if (ul_config_pdu->pdu_type == NFAPI_UL_CONFIG_ULSCH_PDU_TYPE) {
	  LOG_I(MAC, "Panos-D: handle_nfapi_ul_pdu_UE_MAC 2.1 \n");
    //AssertFatal((UE_id = find_ulsch(ul_config_pdu->ulsch_pdu.ulsch_pdu_rel8.rnti,eNB,SEARCH_EXIST_OR_FREE))>=0,
    //            "No existing UE ULSCH for rnti %x\n",rel8->rnti);
    LOG_D(PHY,"Applying UL config for UE, rnti %x for frame %d, subframe %d\n",
         rel8->rnti,frame,subframe);
    uint8_t ulsch_buffer[5477] __attribute__ ((aligned(32)));
    uint16_t buflen = ul_config_pdu->ulsch_pdu.ulsch_pdu_rel8.size;

    uint16_t rnti = ul_config_pdu->ulsch_pdu.ulsch_pdu_rel8.rnti;
    uint8_t access_mode=SCHEDULED_ACCESS;
    if(buflen>0){
    	if(UE_mac_inst[Mod_id].first_ULSCH_Tx == 1){ // Msg3 case
    		LOG_I(MAC, "Panos-D: handle_nfapi_ul_pdu_UE_MAC 2.2 \n");
    		//fill_crc_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, 0);
    		fill_rx_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, UE_mac_inst[Mod_id].RA_prach_resources.Msg3,buflen, rnti);
    		Msg3_transmitted(Mod_id, 0, frame, 0);

    		// Panos: This should be done after the reception of the respective hi_dci0
    		//UE_mac_inst[Mod_id].first_ULSCH_Tx = 0;
    	}
    	else {
    		LOG_I(MAC, "Panos-D: handle_nfapi_ul_pdu_UE_MAC 2.3 \n");
    		ue_get_sdu( Mod_id, 0, frame, subframe, 0, ulsch_buffer, buflen, &access_mode);
    		//fill_crc_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, 0);
    		fill_rx_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, ulsch_buffer,buflen, rnti);
    	}
    }
  }

  else if (ul_config_pdu->pdu_type == NFAPI_UL_CONFIG_ULSCH_HARQ_PDU_TYPE) {
	  LOG_I(MAC, "Panos-D: handle_nfapi_ul_pdu_UE_MAC 3 \n");
    //AssertFatal((UE_id = find_ulsch(ul_config_pdu->ulsch_harq_pdu.ulsch_pdu.ulsch_pdu_rel8.rnti,eNB,SEARCH_EXIST_OR_FREE))>=0,
    //            "No available UE ULSCH for rnti %x\n",ul_config_pdu->ulsch_harq_pdu.ulsch_pdu.ulsch_pdu_rel8.rnti);
	  uint8_t ulsch_buffer[5477] __attribute__ ((aligned(32)));
	  uint16_t buflen = ul_config_pdu->ulsch_harq_pdu.ulsch_pdu.ulsch_pdu_rel8.size;
	  nfapi_ul_config_ulsch_harq_information *ulsch_harq_information = &ul_config_pdu->ulsch_harq_pdu.harq_information;
	  uint16_t rnti = ul_config_pdu->ulsch_harq_pdu.ulsch_pdu.ulsch_pdu_rel8.rnti;
	  uint8_t access_mode=SCHEDULED_ACCESS;
	  if(buflen>0){
		  if(UE_mac_inst[Mod_id].first_ULSCH_Tx == 1){ // Msg3 case
			  fill_crc_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, 0);
			  fill_rx_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, UE_mac_inst[Mod_id].RA_prach_resources.Msg3,buflen, rnti);
			  Msg3_transmitted(Mod_id, 0, frame, 0);
			  //UE_mac_inst[Mod_id].first_ULSCH_Tx = 0;
		  }
		  else {
			  ue_get_sdu( Mod_id, 0, frame, subframe, 0, ulsch_buffer, buflen, &access_mode);
			  fill_crc_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, 0);
			  fill_rx_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, ulsch_buffer,buflen, rnti);
		  }

	  }

	  if(ulsch_harq_information)
		  fill_ulsch_harq_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, ulsch_harq_information, rnti);

  }
  else if (ul_config_pdu->pdu_type == NFAPI_UL_CONFIG_ULSCH_CQI_RI_PDU_TYPE) {
	  LOG_I(MAC, "Panos-D: handle_nfapi_ul_pdu_UE_MAC 4 \n");
    //AssertFatal((UE_id = find_ulsch(ul_config_pdu->ulsch_cqi_ri_pdu.ulsch_pdu.ulsch_pdu_rel8.rnti,
    //                                eNB,SEARCH_EXIST_OR_FREE))>=0,
    //            "No available UE ULSCH for rnti %x\n",ul_config_pdu->ulsch_cqi_ri_pdu.ulsch_pdu.ulsch_pdu_rel8.rnti);
	  uint8_t ulsch_buffer[5477] __attribute__ ((aligned(32)));
	  uint16_t buflen = ul_config_pdu->ulsch_cqi_ri_pdu.ulsch_pdu.ulsch_pdu_rel8.size;

	  uint16_t rnti = ul_config_pdu->ulsch_cqi_ri_pdu.ulsch_pdu.ulsch_pdu_rel8.rnti;
	  uint8_t access_mode=SCHEDULED_ACCESS;
	  if(buflen>0){
		  if(UE_mac_inst[Mod_id].first_ULSCH_Tx == 1){ // Msg3 case
			  fill_crc_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, 0);
			  fill_rx_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, UE_mac_inst[Mod_id].RA_prach_resources.Msg3,buflen, rnti);
			  Msg3_transmitted(Mod_id, 0, frame, 0);
			  //UE_mac_inst[Mod_id].first_ULSCH_Tx = 0;
		  }
		  else {
			  ue_get_sdu( Mod_id, 0, frame, subframe, 0, ulsch_buffer, buflen, &access_mode);
			  fill_crc_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, 0);
			  fill_rx_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, ulsch_buffer,buflen, rnti);
		  }
	  }
	  fill_ulsch_cqi_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, rnti);

  }
  else if (ul_config_pdu->pdu_type == NFAPI_UL_CONFIG_ULSCH_CQI_HARQ_RI_PDU_TYPE) {
	  LOG_I(MAC, "Panos-D: handle_nfapi_ul_pdu_UE_MAC 5 \n");
    //AssertFatal((UE_id = find_ulsch(ul_config_pdu->ulsch_cqi_harq_ri_pdu.ulsch_pdu.ulsch_pdu_rel8.rnti,
    //                                eNB,SEARCH_EXIST_OR_FREE))>=0,
    //            "No available UE ULSCH for rnti %x\n",ul_config_pdu->ulsch_cqi_harq_ri_pdu.ulsch_pdu.ulsch_pdu_rel8.rnti);

	  uint8_t ulsch_buffer[5477] __attribute__ ((aligned(32)));
	  uint16_t buflen = ul_config_pdu->ulsch_cqi_harq_ri_pdu.ulsch_pdu.ulsch_pdu_rel8.size;
	  nfapi_ul_config_ulsch_harq_information *ulsch_harq_information = &ul_config_pdu->ulsch_cqi_harq_ri_pdu.harq_information;

	  uint16_t rnti = ul_config_pdu->ulsch_cqi_harq_ri_pdu.ulsch_pdu.ulsch_pdu_rel8.rnti;
	  uint8_t access_mode=SCHEDULED_ACCESS;
	  if(buflen>0){
		  if(UE_mac_inst[Mod_id].first_ULSCH_Tx == 1){ // Msg3 case
			  fill_crc_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, 0);
			  fill_rx_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, UE_mac_inst[Mod_id].RA_prach_resources.Msg3,buflen, rnti);
			  Msg3_transmitted(Mod_id, 0, frame, 0);
			  //UE_mac_inst[Mod_id].first_ULSCH_Tx = 0;
		  }
		  else {
			  ue_get_sdu( Mod_id, 0, frame, subframe, 0, ulsch_buffer, buflen, &access_mode);
			  fill_crc_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, 0);
			  fill_rx_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, ulsch_buffer,buflen, rnti);
		  }
	  }

	  if(ulsch_harq_information)
		  fill_ulsch_harq_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, ulsch_harq_information, rnti);
	  fill_ulsch_cqi_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, rnti);

  }
  else if (ul_config_pdu->pdu_type == NFAPI_UL_CONFIG_UCI_HARQ_PDU_TYPE) {
	  LOG_I(MAC, "Panos-D: handle_nfapi_ul_pdu_UE_MAC 6 \n");
  //  AssertFatal((UE_id = find_uci(ul_config_pdu->uci_harq_pdu.ue_information.ue_information_rel8.rnti,
  //                                proc->frame_tx,proc->subframe_tx,eNB,SEARCH_EXIST_OR_FREE))>=0,
  //              "No available UE UCI for rnti %x\n",ul_config_pdu->uci_harq_pdu.ue_information.ue_information_rel8.rnti);

	  uint16_t rnti = ul_config_pdu->uci_harq_pdu.ue_information.ue_information_rel8.rnti;

	  nfapi_ul_config_harq_information *ulsch_harq_information = &ul_config_pdu->uci_harq_pdu.harq_information;

	  fill_uci_harq_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO,ulsch_harq_information, rnti);
  }
  else if (ul_config_pdu->pdu_type == NFAPI_UL_CONFIG_UCI_CQI_PDU_TYPE) {
    AssertFatal(1==0,"NFAPI_UL_CONFIG_UCI_CQI_PDU_TYPE not handled yet\n");
  }
  else if (ul_config_pdu->pdu_type == NFAPI_UL_CONFIG_UCI_CQI_HARQ_PDU_TYPE) {
    AssertFatal(1==0,"NFAPI_UL_CONFIG_UCI_CQI_HARQ_PDU_TYPE not handled yet\n");
  }
  else if (ul_config_pdu->pdu_type == NFAPI_UL_CONFIG_UCI_CQI_SR_PDU_TYPE) {
    AssertFatal(1==0,"NFAPI_UL_CONFIG_UCI_CQI_SR_PDU_TYPE not handled yet\n");
  }
  else if (ul_config_pdu->pdu_type == NFAPI_UL_CONFIG_UCI_SR_PDU_TYPE) {
	  LOG_I(MAC, "Panos-D: handle_nfapi_ul_pdu_UE_MAC 7 \n");
    //AssertFatal((UE_id = find_uci(ul_config_pdu->uci_sr_pdu.ue_information.ue_information_rel8.rnti,
    //                              proc->frame_tx,proc->subframe_tx,eNB,SEARCH_EXIST_OR_FREE))>=0,
    //            "No available UE UCI for rnti %x\n",ul_config_pdu->uci_sr_pdu.ue_information.ue_information_rel8.rnti);
	  uint16_t rnti = ul_config_pdu->uci_sr_pdu.ue_information.ue_information_rel8.rnti;

	  fill_sr_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO, rnti);

  }
  else if (ul_config_pdu->pdu_type == NFAPI_UL_CONFIG_UCI_SR_HARQ_PDU_TYPE) {
	  LOG_I(MAC, "Panos-D: handle_nfapi_ul_pdu_UE_MAC 8 \n");
    //AssertFatal((UE_id = find_uci(rel8->rnti,proc->frame_tx,proc->subframe_tx,eNB,SEARCH_EXIST_OR_FREE))>=0,
    //            "No available UE UCI for rnti %x\n",ul_config_pdu->uci_sr_harq_pdu.ue_information.ue_information_rel8.rnti);

	  uint16_t rnti = ul_config_pdu->uci_sr_harq_pdu.ue_information.ue_information_rel8.rnti;

	  // We fill the sr_indication only if ue_get_sr() would normally instruct PHY to send a SR.
	  if (ue_get_SR(Mod_id ,0,frame, 0, rnti, subframe))
		  fill_sr_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO,rnti);

	  nfapi_ul_config_harq_information *ulsch_harq_information = &ul_config_pdu->uci_sr_harq_pdu.harq_information;
	  fill_uci_harq_indication_UE_MAC(Mod_id, frame, subframe, UL_INFO,ulsch_harq_information, rnti);

  }
  /*else if (ul_config_pdu->pdu_type == NFAPI_UL_CONFIG_SRS_PDU_TYPE) {
    handle_srs_pdu(eNB,ul_config_pdu,frame,subframe);
  }*/
}






int ul_config_req_UE_MAC(nfapi_ul_config_request_t* req)
{
  LOG_D(PHY,"[PNF] UL_CONFIG_REQ %s() sfn_sf:%d pdu:%d rach_prach_frequency_resources:%d srs_present:%u\n",
      __FUNCTION__,
      NFAPI_SFNSF2DEC(req->sfn_sf),
      req->ul_config_request_body.number_of_pdus,
      req->ul_config_request_body.rach_prach_frequency_resources,
      req->ul_config_request_body.srs_present
      );

  /*if (RC.ru == 0)
  {
    return -1;
  }

  if (RC.eNB == 0)
  {
    return -2;
  }

  if (RC.eNB[0][0] == 0)
  {
    return -3;
  }

  if (sync_var != 0)
  {
    NFAPI_TRACE(NFAPI_TRACE_INFO, "%s() Main system not up - is this a dummy subframe?\n", __FUNCTION__);
    return -4;
  }*/

  int sfn = NFAPI_SFNSF2SFN(req->sfn_sf);
  int sf = NFAPI_SFNSF2SF(req->sfn_sf);

  //struct PHY_VARS_eNB_s *eNB = RC.eNB[0][0];
  //eNB_rxtx_proc_t *proc = &eNB->proc.proc_rxtx[0];

  module_id_t Mod_id = 0; //Panos: Currently static (only for one UE) but this should change.
  nfapi_ul_config_request_pdu_t* ul_config_pdu_list = req->ul_config_request_body.ul_config_pdu_list;
  LOG_I(MAC, "Panos-D: ul_config_req_UE_MAC 1 \n");

  //Panos: Not sure whether we should put the memory allocation here.
  //*** Note we should find the right place to call free(UL_INFO).
  UL_INFO = (UL_IND_t*)malloc(sizeof(UL_IND_t));

  //Panos: Additional checks needed here to check if the UE is in PRACH mode.

  /*uint8_t is_rach = req->ul_config_request_body.rach_prach_frequency_resources;
  if(is_rach && UE_mac_inst[Mod_id].UE_mode[0] == PRACH) {
	  PRACH_RESOURCES_t *prach_resources = ue_get_rach(Mod_id, 0, sfn, 0, sf);
	  fill_rach_indication_UE_MAC(Mod_id, sfn ,sf, UL_INFO, prach_resources->ra_PreambleIndex, prach_resources->ra_RNTI);
	  Msg1_transmitted(Mod_id, 0, sfn, 0);
  }*/


  // subframe works off TX SFN/SF which is 4 ahead, need to put it back to RX SFN/SF
  // probably could just use proc->frame_rx

  // PanosQ: This an eNB MAC function. Are we allowed to call it from here?
  // Also, it is only in the nfapi-RU-RAU-split
  //subtract_subframe(&sfn, &sf, 4);



  for (int i=0;i<req->ul_config_request_body.number_of_pdus;i++)
  {
    //NFAPI_TRACE(NFAPI_TRACE_INFO, "%s() sfn/sf:%d PDU[%d] size:%d\n", __FUNCTION__, NFAPI_SFNSF2DEC(req->sfn_sf), i, ul_config_pdu_list[i].pdu_size);

	LOG_I(MAC, "Panos-D: ul_config_req_UE_MAC 2.0 \n");
    if (
    		req->ul_config_request_body.ul_config_pdu_list[i].pdu_type == NFAPI_UL_CONFIG_ULSCH_PDU_TYPE ||
    		req->ul_config_request_body.ul_config_pdu_list[i].pdu_type == NFAPI_UL_CONFIG_ULSCH_HARQ_PDU_TYPE ||
    		req->ul_config_request_body.ul_config_pdu_list[i].pdu_type == NFAPI_UL_CONFIG_ULSCH_CQI_RI_PDU_TYPE ||
    		req->ul_config_request_body.ul_config_pdu_list[i].pdu_type == NFAPI_UL_CONFIG_ULSCH_CQI_HARQ_RI_PDU_TYPE ||
    		req->ul_config_request_body.ul_config_pdu_list[i].pdu_type == NFAPI_UL_CONFIG_UCI_HARQ_PDU_TYPE ||
    		req->ul_config_request_body.ul_config_pdu_list[i].pdu_type == NFAPI_UL_CONFIG_UCI_SR_PDU_TYPE ||
    		req->ul_config_request_body.ul_config_pdu_list[i].pdu_type == NFAPI_UL_CONFIG_UCI_SR_HARQ_PDU_TYPE
       )
    {
      //NFAPI_TRACE(NFAPI_TRACE_INFO, "%s() handle_nfapi_ul_pdu() for PDU:%d\n", __FUNCTION__, i);

    	LOG_I(MAC, "Panos-D: ul_config_req_UE_MAC 2.1 \n");
      handle_nfapi_ul_pdu_UE_MAC(Mod_id,&req->ul_config_request_body.ul_config_pdu_list[i],sfn,sf,req->ul_config_request_body.srs_present);
      LOG_I(MAC, "Panos-D: ul_config_req_UE_MAC 3 \n");
    }
    else
    {
      //NFAPI_TRACE(NFAPI_TRACE_ERROR, "%s() PDU:%i UNKNOWN type :%d\n", __FUNCTION__, i, ul_config_pdu_list[i].pdu_type);
    }
  }
  free(UL_INFO);

  return 0;
}

int tx_req_UE_MAC1( int k) {
	return 0;
}


int tx_req_UE_MAC(nfapi_tx_request_t* req)
{
  uint16_t sfn = NFAPI_SFNSF2SFN(req->sfn_sf);
  uint16_t sf = NFAPI_SFNSF2SF(req->sfn_sf);

  LOG_D(PHY,"%s() SFN/SF:%d/%d PDUs:%d\n", __FUNCTION__, sfn, sf, req->tx_request_body.number_of_pdus);

  //LOG_I(MAC, "Panos-D: tx_req_UE_MAC 1 \n");
  //printf("Panos-D: tx_req_UE_MAC 1 \n");
  //if (req->tx_request_body.tl.tag==NFAPI_TX_REQUEST_BODY_TAG)
  //{

    for (int i=0; i<req->tx_request_body.number_of_pdus; i++)
    {
      LOG_D(PHY,"%s() SFN/SF:%d/%d number_of_pdus:%d [PDU:%d] pdu_length:%d pdu_index:%d num_segments:%d\n",
          __FUNCTION__,
          sfn, sf,
          req->tx_request_body.number_of_pdus,
          i,
          req->tx_request_body.tx_pdu_list[i].pdu_length,
          req->tx_request_body.tx_pdu_list[i].pdu_index,
          req->tx_request_body.tx_pdu_list[i].num_segments
          );

      //tx_request_pdu_list = req->tx_request_body.tx_pdu_list;
      //tx_request_pdu[sfn][sf][i] = &req->tx_request_body.tx_pdu_list[i];
    }
  //}

  return 0;
}


int dl_config_req_UE_MAC(nfapi_dl_config_request_t* req)
{
	if (req!=NULL && tx_request_pdu_list!=NULL){
	//LOG_I(MAC, "Panos-D: dl_config_req_UE_MAC 1 \n");
  int sfn = NFAPI_SFNSF2SFN(req->sfn_sf);
  int sf = NFAPI_SFNSF2SF(req->sfn_sf);
  module_id_t Mod_id = 0; //Panos: Currently static (only for one UE) but this should change.

  /*struct PHY_VARS_eNB_s *eNB = RC.eNB[0][0];
  eNB_rxtx_proc_t *proc = &eNB->proc.proc_rxtx[0];*/
  nfapi_dl_config_request_pdu_t* dl_config_pdu_list = req->dl_config_request_body.dl_config_pdu_list;
  nfapi_dl_config_request_pdu_t *dl_config_pdu_tmp;

  /*LTE_eNB_PDCCH *pdcch_vars = &eNB->pdcch_vars[sf&1];

  pdcch_vars->num_pdcch_symbols = req->dl_config_request_body.number_pdcch_ofdm_symbols;
  pdcch_vars->num_dci = 0;*/

  //NFAPI_TRACE(NFAPI_TRACE_INFO, "%s() TX:%d/%d RX:%d/%d sfn_sf:%d DCI:%d PDU:%d\n", __FUNCTION__, proc->frame_tx, proc->subframe_tx, proc->frame_rx, proc->subframe_rx, NFAPI_SFNSF2DEC(req->sfn_sf), req->dl_config_request_body.number_dci, req->dl_config_request_body.number_pdu);



  //LOG_D(PHY,"NFAPI: Sched_INFO:SFN/SF:%d%d dl_pdu:%d tx_req:%d hi_dci0:%d ul_cfg:%d num_pdcch_symbols:%d\n",
  //	frame,subframe,number_dl_pdu,TX_req->tx_request_body.number_of_pdus,number_hi_dci0_pdu,number_ul_pdu, eNB->pdcch_vars[subframe&1].num_pdcch_symbols);

  for (int i=0;i<req->dl_config_request_body.number_pdu;i++)
  {
    //NFAPI_TRACE(NFAPI_TRACE_INFO, "%s() sfn/sf:%d PDU[%d] size:%d\n", __FUNCTION__, NFAPI_SFNSF2DEC(req->sfn_sf), i, dl_config_pdu_list[i].pdu_size);
	  LOG_E(MAC, "dl_config_req_UE_MAC 2 Received real ones: sfn/sf:%d.%d PDU[%d] size:%d\n", sfn, sf, i, dl_config_pdu_list[i].pdu_size);

    if (dl_config_pdu_list[i].pdu_type == NFAPI_DL_CONFIG_DCI_DL_PDU_TYPE)
    {
		if (dl_config_pdu_list[i].dci_dl_pdu.dci_dl_pdu_rel8.rnti_type == 1) {
			// C-RNTI (Normal DLSCH case)
			dl_config_pdu_tmp = &dl_config_pdu_list[i+1];
			if (dl_config_pdu_tmp->pdu_type == NFAPI_DL_CONFIG_DLSCH_PDU_TYPE){
				if(tx_request_pdu_list + dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index!= NULL){
					LOG_E(MAC, "dl_config_req_UE_MAC 2 Received data: sfn/sf:%d PDU[%d] size:%d\n", NFAPI_SFNSF2DEC(req->sfn_sf), i, dl_config_pdu_list[i].pdu_size);
					ue_send_sdu(Mod_id, 0, sfn, sf,
							tx_request_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data,
							tx_request_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_length,
							0);
					i++;
				}
			}
			else {
				LOG_E(MAC,"[UE %d] Frame %d, subframe %d : Cannot extract DLSCH PDU from NFAPI\n",Mod_id, sfn,sf);
			}
		}
		else if (dl_config_pdu_list[i].dci_dl_pdu.dci_dl_pdu_rel8.rnti_type == 2) {
			dl_config_pdu_tmp = &dl_config_pdu_list[i+1];
			if(dl_config_pdu_tmp->pdu_type == NFAPI_DL_CONFIG_DLSCH_PDU_TYPE && dl_config_pdu_list[i].dci_dl_pdu.dci_dl_pdu_rel8.rnti == 0xFFFF){
				//pdu = Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data;
				LOG_E(MAC,"dl_config_req_UE_MAC 3 Received SI: [PDU:%d] NFAPI_DL_CONFIG_DLSCH_PDU_TYPE TX:%d/%d RX:%d/%d transport_blocks:%d pdu_index:%d sdu:%p\n",
				            i, sfn, sf, sfn, sf, dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.transport_blocks, dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index, tx_request_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data);
				if(tx_request_pdu_list + dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index!= NULL){
					ue_decode_si(Mod_id, 0, sfn, 0,
							tx_request_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data,
							tx_request_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_length);
					i++;
				}
			}
			else if(dl_config_pdu_tmp->pdu_type == NFAPI_DL_CONFIG_DLSCH_PDU_TYPE && dl_config_pdu_list[i].dci_dl_pdu.dci_dl_pdu_rel8.rnti == 0xFFFE){
				// P_RNTI case
				//pdu = Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data;
				if(tx_request_pdu_list + dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index!= NULL){
					ue_decode_p(Mod_id, 0, sfn, 0,
							tx_request_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data,
							tx_request_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_length);
					i++;
				}
			}
			else if(dl_config_pdu_tmp->pdu_type == NFAPI_DL_CONFIG_DLSCH_PDU_TYPE) {
				// RA-RNTI case
				LOG_E(MAC,"dl_config_req_UE_MAC 4 Received RAR? \n");
				// RNTI parameter not actually used. Provided only to comply with existing function definition.
				// Not sure about parameters to fill the preamble index.
				//rnti_t c_rnti = UE_mac_inst[Mod_id].crnti;
				rnti_t ra_rnti = UE_mac_inst[Mod_id].RA_prach_resources.ra_RNTI;
				if ((UE_mac_inst[Mod_id].UE_mode[0] != PUSCH) &&
				  (UE_mac_inst[Mod_id].RA_prach_resources.Msg3!=NULL) &&
				  (tx_request_pdu_list + dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index!= NULL)) {
					LOG_E(MAC,"dl_config_req_UE_MAC 5 Received RAR \n");
					ue_process_rar(Mod_id, 0, sfn,
							ra_rnti, //RA-RNTI
							tx_request_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data,
							&dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.rnti, //t-crnti
							UE_mac_inst[Mod_id].RA_prach_resources.ra_PreambleIndex,
							tx_request_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data);
					UE_mac_inst[Mod_id].UE_mode[0] = RA_RESPONSE;
					UE_mac_inst[Mod_id].first_ULSCH_Tx = 1; //Expecting an UL_CONFIG_ULSCH_PDU to enable Msg3 Txon (first ULSCH Txon for the UE)
				}
			}
			else {
				LOG_E(MAC,"[UE %d] %d Frame %d, subframe %d : Cannot extract DLSCH PDU from NFAPI\n",Mod_id, sfn, sf);
			}

		}
    }
    else if (dl_config_pdu_list[i].pdu_type == NFAPI_DL_CONFIG_BCH_PDU_TYPE)
    {
    	// BCH case
    	// Last parameter is 1 if first time synchronization and zero otherwise. Not sure which value to put
    	// for our case.
    	LOG_E(MAC,"dl_config_req_UE_MAC 4 Received MIB: sfn/sf: %d.%d \n", sfn, sf);
    	dl_phy_sync_success(Mod_id,sfn,0, 0);
    	if(UE_mac_inst[Mod_id].UE_mode[0] == NOT_SYNCHED){
    		LOG_E(MAC,"dl_config_req_UE_MAC 5 Received MIB: UE_mode: %d\n", UE_mac_inst[Mod_id].UE_mode[0]);
    		UE_mac_inst[Mod_id].UE_mode[0]=PRACH;
    	}

    }

    else
    {
      //NFAPI_TRACE(NFAPI_TRACE_ERROR, "%s() UNKNOWN:%d\n", __FUNCTION__, dl_config_pdu_list[i].pdu_type);
    }
  }
  //deallocate_mem_nfapi_dl

  if(req->vendor_extension)
    free(req->vendor_extension);

  /*if(req->vendor_extension!=NULL){
    free(req->vendor_extension);
    req->vendor_extension = NULL;
  }*/

  if(tx_request_pdu_list!=NULL){
	  free(tx_request_pdu_list);
  	  tx_request_pdu_list = NULL;
  }
  free(req);
  req = NULL;
  return 0;
	}
	else {
		return -1;
	}




}


int deallocate_mem_nfapi_dl(nfapi_dl_config_request_t* req, nfapi_tx_request_pdu_t* tx_pdu_list){

}




int hi_dci0_req_UE_MAC(nfapi_hi_dci0_request_t* req)
{
  LOG_D(PHY,"[UE-PHY_STUB] hi dci0 request sfn_sf:%d number_of_dci:%d number_of_hi:%d\n", NFAPI_SFNSF2DEC(req->sfn_sf), req->hi_dci0_request_body.number_of_dci, req->hi_dci0_request_body.number_of_hi);

  //phy_info* phy = (phy_info*)(pnf_p7->user_data);

  module_id_t Mod_id = 0; //Panos: Currently static (only for one UE) but this should change.

  /*struct PHY_VARS_eNB_s *eNB = RC.eNB[0][0];
  eNB_rxtx_proc_t *proc = &eNB->proc.proc_rxtx[0];*/

  for (int i=0; i<req->hi_dci0_request_body.number_of_dci + req->hi_dci0_request_body.number_of_hi; i++)
  {
    LOG_D(PHY,"[UE-PHY_STUB] HI_DCI0_REQ sfn_sf:%d PDU[%d]\n", NFAPI_SFNSF2DEC(req->sfn_sf), i);

    if (req->hi_dci0_request_body.hi_dci0_pdu_list[i].pdu_type == NFAPI_HI_DCI0_DCI_PDU_TYPE)
    {
      LOG_D(PHY,"[UE-PHY_STUB] HI_DCI0_REQ sfn_sf:%d PDU[%d] - NFAPI_HI_DCI0_DCI_PDU_TYPE not used \n", NFAPI_SFNSF2DEC(req->sfn_sf), i);

    }
    else if (req->hi_dci0_request_body.hi_dci0_pdu_list[i].pdu_type == NFAPI_HI_DCI0_HI_PDU_TYPE)
    {
      LOG_D(PHY,"[UE-PHY_STUB] HI_DCI0_REQ sfn_sf:%d PDU[%d] - NFAPI_HI_DCI0_HI_PDU_TYPE\n", NFAPI_SFNSF2DEC(req->sfn_sf), i);

      nfapi_hi_dci0_request_pdu_t *hi_dci0_req_pdu = &req->hi_dci0_request_body.hi_dci0_pdu_list[i];

      // This is meaningful only after ACKnowledging the first ULSCH Txon (i.e. Msg3)
      if(hi_dci0_req_pdu->hi_pdu.hi_pdu_rel8.hi_value == 1 && UE_mac_inst[Mod_id].first_ULSCH_Tx == 1){
    	  UE_mac_inst[Mod_id].UE_mode[0] = PUSCH;
    	  UE_mac_inst[Mod_id].first_ULSCH_Tx = 0;
      }

    }
    else
    {
      LOG_E(PHY,"[UE-PHY_STUB] HI_DCI0_REQ sfn_sf:%d PDU[%d] - unknown pdu type:%d\n", NFAPI_SFNSF2DEC(req->sfn_sf), i, req->hi_dci0_request_body.hi_dci0_pdu_list[i].pdu_type);
    }
  }

  return 0;
}





// The following set of memcpy functions should be getting called as callback functions from
// pnf_p7_subframe_ind.
int memcpy_dl_config_req (nfapi_pnf_p7_config_t* pnf_p7, nfapi_dl_config_request_t* req)
{

	module_id_t Mod_id = 0; //Panos: Currently static (only for one UE) but this should change.
	UE_mac_inst[Mod_id].dl_config_req = (nfapi_dl_config_request_t*)malloc(sizeof(nfapi_dl_config_request_t));
	LOG_I(MAC, "Panos-D: memcpy_dl_config_req 1 \n");


	//UE_mac_inst[Mod_id].dl_config_req->header = req->header;
	UE_mac_inst[Mod_id].dl_config_req->sfn_sf = req->sfn_sf;
	//vendor_extension = &UE_mac_inst[Mod_id].dl_config_req->vendor_extension;
	//vendor_extension->tag = req->vendor_extension.tag;
	//vendor_extension->length = req->vendor_extension.length;

	UE_mac_inst[Mod_id].dl_config_req->vendor_extension = req->vendor_extension;
	//UE_mac_inst[Mod_id].dl_config_req->vendor_extension.tag = req->vendor_extension.tag;
	//UE_mac_inst[Mod_id].dl_config_req->vendor_extension.length = req->vendor_extension.length;

	UE_mac_inst[Mod_id].dl_config_req->dl_config_request_body.number_dci = req->dl_config_request_body.number_dci;
	UE_mac_inst[Mod_id].dl_config_req->dl_config_request_body.number_pdcch_ofdm_symbols = req->dl_config_request_body.number_pdcch_ofdm_symbols;
	UE_mac_inst[Mod_id].dl_config_req->dl_config_request_body.number_pdsch_rnti = req->dl_config_request_body.number_pdsch_rnti;
	UE_mac_inst[Mod_id].dl_config_req->dl_config_request_body.number_pdu = req->dl_config_request_body.number_pdu;
	UE_mac_inst[Mod_id].dl_config_req->dl_config_request_body.tl = req->dl_config_request_body.tl;

	UE_mac_inst[Mod_id].dl_config_req->dl_config_request_body.dl_config_pdu_list = (nfapi_dl_config_request_pdu_t*) malloc(req->dl_config_request_body.number_pdu*sizeof(nfapi_dl_config_request_pdu_t));
	for(int i=0; i<UE_mac_inst[Mod_id].dl_config_req->dl_config_request_body.number_pdu; i++) {
		UE_mac_inst[Mod_id].dl_config_req->dl_config_request_body.dl_config_pdu_list[i] = req->dl_config_request_body.dl_config_pdu_list[i];
	}

	//UE_mac_inst[Mod_id].dl_config_req = req;
	return 0;

}

int memcpy_ul_config_req (nfapi_pnf_p7_config_t* pnf_p7, nfapi_ul_config_request_t* req)
{
	LOG_I(MAC, "Panos-D: memcpy_ul_config_req 1 \n");

	module_id_t Mod_id = 0; //Panos: Currently static (only for one UE) but this should change.

	UE_mac_inst[Mod_id].ul_config_req = (nfapi_ul_config_request_t*)malloc(sizeof(nfapi_ul_config_request_t));
	//UE_mac_inst[Mod_id].ul_config_req->header = req->header;
	UE_mac_inst[Mod_id].ul_config_req->sfn_sf = req->sfn_sf;
	UE_mac_inst[Mod_id].ul_config_req->vendor_extension = req->vendor_extension;
	//UE_mac_inst[Mod_id].ul_config_req->vendor_extension->tag = req->vendor_extension.tag;
	//UE_mac_inst[Mod_id].ul_config_req->vendor_extension->length = req->vendor_extension.length;

	LOG_I(MAC, "Panos-D: memcpy_ul_config_req SFN_SF: MINE: %d \n", UE_mac_inst[Mod_id].ul_config_req->sfn_sf);
	LOG_I(MAC, "Panos-D: memcpy_ul_config_req SFN_SF: REQ: %d \n", req->sfn_sf);

	UE_mac_inst[Mod_id].ul_config_req->ul_config_request_body.number_of_pdus = req->ul_config_request_body.number_of_pdus;
	UE_mac_inst[Mod_id].ul_config_req->ul_config_request_body.rach_prach_frequency_resources = req->ul_config_request_body.rach_prach_frequency_resources;
	UE_mac_inst[Mod_id].ul_config_req->ul_config_request_body.srs_present = req->ul_config_request_body.srs_present;
	UE_mac_inst[Mod_id].ul_config_req->ul_config_request_body.tl = req->ul_config_request_body.tl;

	LOG_I(MAC, "Panos-D: memcpy_ul_config_req 1 #ofULPDUs: %d \n", UE_mac_inst[Mod_id].ul_config_req->ul_config_request_body.number_of_pdus); //req->ul_config_request_body.number_of_pdus);
	UE_mac_inst[Mod_id].ul_config_req->ul_config_request_body.ul_config_pdu_list = (nfapi_ul_config_request_pdu_t*) malloc(req->ul_config_request_body.number_of_pdus*sizeof(nfapi_ul_config_request_pdu_t));
	for(int i=0; i<UE_mac_inst[Mod_id].ul_config_req->ul_config_request_body.number_of_pdus; i++) {
			UE_mac_inst[Mod_id].ul_config_req->ul_config_request_body.ul_config_pdu_list[i] = req->ul_config_request_body.ul_config_pdu_list[i];
			LOG_I(MAC, "Panos-D: memcpy_ul_config_req TAG MINE: %d \n", UE_mac_inst[Mod_id].ul_config_req->ul_config_request_body.ul_config_pdu_list[i].pdu_type);
			LOG_I(MAC, "Panos-D: memcpy_ul_config_req TAG REQ: %d \n", req->ul_config_request_body.ul_config_pdu_list[i].pdu_type);
		}


	//UE_mac_inst[Mod_id].ul_config_req = req;
	return 0;
}

int memcpy_tx_req (nfapi_pnf_p7_config_t* pnf_p7, nfapi_tx_request_t* req)
{
	module_id_t Mod_id = 0; //Panos: Currently static (only for one UE) but this should change.
	//LOG_I(MAC, "Panos-D: memcpy_tx_req 1, req->tx_request_body.number_of_pdus: %d \n", req->tx_request_body.number_of_pdus);
	//LOG_I(MAC, "Panos-D: memcpy_tx_req 1, req->tx_request_body.tx_pdu_list[i].pdu_length: %d \n", req->tx_request_body.tx_pdu_list[0].pdu_length);
	//LOG_I(MAC, "Panos-D: memcpy_tx_req 1, req->tx_request_body.tx_pdu_list[i].pdu_index: %d \n", req->tx_request_body.tx_pdu_list[0].pdu_index);
	//LOG_I(MAC, "Panos-D: memcpy_tx_req 1, req->tx_request_body.tx_pdu_list[i].num_segments: %d \n", req->tx_request_body.tx_pdu_list[0].num_segments);
	//LOG_I(MAC, "Panos-D: memcpy_tx_req 1, req->tx_request_body.tx_pdu_list[i].segments[j].segment_data: %d \n", *req->tx_request_body.tx_pdu_list[0].segments[0].segment_data);
	//printf("Panos-D: memcpy_tx_req 1, req->tx_request_body.number_of_pdus: %d \n", req->tx_request_body.number_of_pdus);
	//printf("Panos-D: memcpy_tx_req 1, req->tx_request_body.tx_pdu_list[i].pdu_length: %d \n", req->tx_request_body.tx_pdu_list[0].pdu_length);
	//printf("Panos-D: memcpy_tx_req 1, req->tx_request_body.tx_pdu_list[i].pdu_index: %d \n", req->tx_request_body.tx_pdu_list[0].pdu_index);
	//printf("Panos-D: memcpy_tx_req 1, req->tx_request_body.tx_pdu_list[i].num_segments: %d \n", req->tx_request_body.tx_pdu_list[0].num_segments);
	//printf("Panos-D: memcpy_tx_req 1, req->tx_request_body.tx_pdu_list[i].segments[j].segment_data: %d \n", *req->tx_request_body.tx_pdu_list[0].segments[0].segment_data);



	int num_elem = req->tx_request_body.number_of_pdus;
	tx_request_pdu_list = (nfapi_tx_request_pdu_t*) malloc(num_elem*sizeof(nfapi_tx_request_pdu_t));
	//UE_mac_inst[Mod_id].tx_req = (nfapi_tx_request_t*) malloc(sizeof(nfapi_tx_request_t));
	//memcpy(UE_mac_inst[Mod_id].tx_req, req, sizeof(req));
	for (int i=0; i<num_elem; i++) {
		tx_request_pdu_list[i].num_segments = req->tx_request_body.tx_pdu_list[i].num_segments;
		tx_request_pdu_list[i].pdu_index = req->tx_request_body.tx_pdu_list[i].pdu_index;
		tx_request_pdu_list[i].pdu_length = req->tx_request_body.tx_pdu_list[i].pdu_length;
		for (int j=0; j<req->tx_request_body.tx_pdu_list[i].num_segments; j++){
			//*tx_request_pdu_list[i].segments[j].segment_data = *req->tx_request_body.tx_pdu_list[i].segments[j].segment_data;
			tx_request_pdu_list[i].segments[j].segment_length = req->tx_request_body.tx_pdu_list[i].segments[j].segment_length;
			if(tx_request_pdu_list[i].segments[j].segment_length > 0){
			tx_request_pdu_list[i].segments[j].segment_data = (uint8_t*)malloc(tx_request_pdu_list[i].segments[j].segment_length*sizeof (uint8_t));
			memcpy(tx_request_pdu_list[i].segments[j].segment_data, req->tx_request_body.tx_pdu_list[i].segments[j].segment_data, tx_request_pdu_list[i].segments[j].segment_length);
			}
			//tx_request_pdu_list[i].segments[j].segment_length = req->tx_request_body.tx_pdu_list[i].segments[j].segment_length;
		}

		//tx_request_pdu_list[i].segments = req->tx_request_body.tx_pdu_list[i].segments;
	}


	// Panos: Old way. Not possible to use because by the time we call tx_req_UE_MAC tx_req memory has been deallocated within nfapi.
	//UE_mac_inst[Mod_id].tx_req = req;
	return 0;
}

int memcpy_hi_dci0_req (nfapi_pnf_p7_config_t* pnf_p7, nfapi_hi_dci0_request_t* req)
{
	module_id_t Mod_id = 0; //Panos: Currently static (only for one UE) but this should change.
	UE_mac_inst[Mod_id].hi_dci0_req = req;
	return 0;
}



/*void handle_nfapi_UE_Rx(uint8_t Mod_id, Sched_Rsp_t *Sched_INFO, int eNB_id){
	// copy data from eNB L2 interface to UE L2 interface

	int 					   CC_id	   = Sched_INFO->CC_id;
	nfapi_dl_config_request_t *DL_req      = Sched_INFO->DL_req;
	nfapi_tx_request_t        *Tx_req      = Sched_INFO->TX_req;
	frame_t                   frame        = Sched_INFO->frame;
	sub_frame_t               subframe     = Sched_INFO->subframe;

	uint8_t number_dl_pdu             = DL_req->dl_config_request_body.number_pdu;
	nfapi_dl_config_request_pdu_t *dl_config_pdu;
	nfapi_dl_config_request_pdu_t *dl_config_pdu_tmp;
	int i = 0;

	for (i=0; i<number_dl_pdu; i++)
	{
		dl_config_pdu = &DL_req->dl_config_request_body.dl_config_pdu_list[i];
		switch (dl_config_pdu->pdu_type) {
		case NFAPI_DL_CONFIG_BCH_PDU_TYPE:
			// BCH case
			// Last parameter is 1 if first time synchronization and zero otherwise. Not sure which value to put
			// for our case.
			dl_phy_sync_success(Mod_id,frame,eNB_id, 0);
			break;

		case NFAPI_DL_CONFIG_DCI_DL_PDU_TYPE:
			if (dl_config_pdu->dci_dl_pdu.dci_dl_pdu_rel8.rnti_type == 1) {
				// C-RNTI (Normal DLSCH case)
				dl_config_pdu_tmp = &DL_req->dl_config_request_body.dl_config_pdu_list[i+1];
				if (dl_config_pdu_tmp->pdu_type == NFAPI_DL_CONFIG_DLSCH_PDU_TYPE){
					ue_send_sdu(Mod_id, CC_id, frame, subframe,
							Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data,
							Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_length,
							eNB_id);
					i++;
				}
				else {
					LOG_E(MAC,"[UE %d] CCid %d Frame %d, subframe %d : Cannot extract DLSCH PDU from NFAPI\n",Mod_id, CC_id,frame,subframe);
				}
			}
			else if (dl_config_pdu->dci_dl_pdu.dci_dl_pdu_rel8.rnti_type == 2) {
				dl_config_pdu_tmp = &DL_req->dl_config_request_body.dl_config_pdu_list[i+1];
				if(dl_config_pdu_tmp->pdu_type == NFAPI_DL_CONFIG_DLSCH_PDU_TYPE && dl_config_pdu->dci_dl_pdu.dci_dl_pdu_rel8.rnti == 0xFFFF){
					//pdu = Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data;
					// Question about eNB_index here. How do we obtain it?
					ue_decode_si(Mod_id, CC_id, frame, eNB_id,
							Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data,
							Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_length);
					i++;
				}
				else if(dl_config_pdu_tmp->pdu_type == NFAPI_DL_CONFIG_DLSCH_PDU_TYPE && dl_config_pdu->dci_dl_pdu.dci_dl_pdu_rel8.rnti == 0xFFFE){
					// P_RNTI case
					//pdu = Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data;
					// Question about eNB_index here. How do we obtain it?
					ue_decode_p(Mod_id, CC_id, frame, eNB_id,
							Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data,
							Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_length);
					i++;
				}
				else if(dl_config_pdu_tmp->pdu_type == NFAPI_DL_CONFIG_DLSCH_PDU_TYPE) {
					// RA-RNTI case
					// ue_process_rar should be called but the problem is that this function currently uses PHY_VARS_UE
					// elements.

					// RNTI parameter not actually used. Provided only to comply with existing function definition.
					// Not sure about parameters to fill the preamble index.
					//rnti_t c_rnti = UE_mac_inst[Mod_id].crnti;
					rnti_t ra_rnti = UE_mac_inst[Mod_id].RA_prach_resources.ra_RNTI;
					if ((UE_mac_inst[Mod_id].UE_mode[0] != PUSCH) &&
					  (UE_mac_inst[Mod_id].RA_prach_resources.Msg3!=NULL)) {
						ue_process_rar(Mod_id, CC_id, frame,
								ra_rnti, //RA-RNTI
								Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data,
								&dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.rnti, //t-crnti
								UE_mac_inst[Mod_id].RA_prach_resources.ra_PreambleIndex,
								Tx_req->tx_request_body.tx_pdu_list[dl_config_pdu_tmp->dlsch_pdu.dlsch_pdu_rel8.pdu_index].segments[0].segment_data);
						UE_mac_inst[Mod_id].UE_mode[0] = RA_RESPONSE;
						UE_mac_inst[Mod_id].first_ULSCH_Tx = 1; //Expecting an UL_CONFIG_ULSCH_PDU to enable Msg3 Txon (first ULSCH Txon for the UE)
					}
				}
				else {
					LOG_E(MAC,"[UE %d] CCid %d Frame %d, subframe %d : Cannot extract DLSCH PDU from NFAPI\n",Mod_id, CC_id,frame,subframe);
				}

			}
			break;
		}

	}


}*/
