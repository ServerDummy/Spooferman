static void send_ECM_chassis_speed(CAN_FIFOMailBox_TypeDef *to_fwd){
    uint16_t spoofSpeed = 0x6504;

    uint8_t lowByte = spoofSpeed & 0xFF;
    uint8_t highByte = (spoofSpeed >> 8) & 0xFF;


    to_fwd->RDLR &= 0x00000000; //clear current value
    to_fwd->RDLR |= spoofSpeed << 8; //Current (filtered) speed
    to_fwd->RDLR |= highByte << 24;
    to_fwd->RDHR &= 0x00000000;
    to_fwd->RDHR |= lowByte;
}

int default_rx_hook(CAN_FIFOMailBox_TypeDef *to_push) {
  UNUSED(to_push);
  return true;
}

// *** no output safety mode ***

static void nooutput_init(int16_t param) {
  UNUSED(param);
  controls_allowed = false;
  relay_malfunction_reset();
}

static int nooutput_tx_hook(CAN_FIFOMailBox_TypeDef *to_send) {
  UNUSED(to_send);
  return false;
}

static int nooutput_tx_lin_hook(int lin_num, uint8_t *data, int len) {
  UNUSED(lin_num);
  UNUSED(data);
  UNUSED(len);
  return false;
}

static int default_fwd_hook(int bus_num, CAN_FIFOMailBox_TypeDef *to_fwd) {
  int addr = GET_ADDR(to_fwd);
  int bus_fwd = -1;
  
  if (bus_num == 0) {
    bus_fwd = 1;
    if (addr == 1001) { //0x3e9
        send_ECM_chassis_speed(to_fwd);
    }
  }
  if (bus_num == 1) {
    bus_fwd = 0;
  }
  return bus_fwd;
}


const safety_hooks nooutput_hooks = {
  .init = nooutput_init,
  .rx = default_rx_hook,
  .tx = nooutput_tx_hook,
  .tx_lin = nooutput_tx_lin_hook,
  .fwd = default_fwd_hook,
};

// *** all output safety mode ***

static void alloutput_init(int16_t param) {
  UNUSED(param);
  controls_allowed = true;
  relay_malfunction_reset();
}

static int alloutput_tx_hook(CAN_FIFOMailBox_TypeDef *to_send) {
  UNUSED(to_send);
  return true;
}

static int alloutput_tx_lin_hook(int lin_num, uint8_t *data, int len) {
  UNUSED(lin_num);
  UNUSED(data);
  UNUSED(len);
  return true;
}

const safety_hooks alloutput_hooks = {
  .init = alloutput_init,
  .rx = default_rx_hook,
  .tx = alloutput_tx_hook,
  .tx_lin = alloutput_tx_lin_hook,
  .fwd = default_fwd_hook,
};
