#include "motor_dm.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


DM4310::DM4310(PCAN* hcan,const Motor_DM_Mode mode, const uint8_t can_id, const uint8_t master_id , const Pid_Type pid_type) :
        pid_speed(0.0f, 0.0f, 0.0f, 0.001f, 0.0f),
        pid_angle(0.0f, 0.0f, 0.0f, 0.001f, 0.0f)
{
    this->mode = mode;
    this->hcan = hcan;
    this->can_id = can_id;
    this->master_id = master_id;
    this->pid_type = pid_type;



}

void DM4310::Set_Angle(float angle, const float speed)
{

    switch(this->mode)
    {
    case(MIT_MODE):
        MIT_Ctl_Msg_Send(angle, speed, 2, 1, 0);
        break;
    case(POSITION_AND_SPEED_MODE):
        Pos_Speed_Msg_Send(angle, speed);
        break;
    case(MIT_TORQUE_MODE):
        if (this->pid_type!=POSITION_LOOP)
        {
            DM_Error_Handler();
            return;
        }
        Pid_Update(angle);
    default:
        DM_Error_Handler();
    }
}

void DM4310::Set_Speed(const float speed)
{
    switch(this->mode)
    {
    case(MIT_MODE):
        MIT_Ctl_Msg_Send(0 ,speed, 0, 1, 0);
        break;
    case(POSITION_AND_SPEED_MODE):
        Pos_Speed_Msg_Send(0, speed);
        break;
    case(SPEED_MODE):
        Speed_Msg_Send(speed);
        break;
    case(MIT_TORQUE_MODE):
        if (this->pid_type!=SPEED_LOOP)
        {
            DM_Error_Handler();
            return;
        }
        Pid_Update(speed);
        break;
    case(NONE_MODE):
        DM_Error_Handler();
        break;

    default:
        DM_Error_Handler();
    }
}

void DM4310::MIT_Ctl_Msg_Send(const float pos , const float vel , const float kp , const float kd , const float torque) const
{
    if (this->mode != MIT_MODE && this->mode != MIT_TORQUE_MODE)
    {
        DM_Error_Handler();
        return;
    }

    uint8_t tx_data[8];

    //计算发送数据
    uint16_t pos_tmp = float_to_uint(pos, P_MIN, P_MAX, 16);
    uint16_t vel_tmp = float_to_uint(vel, V_MIN, V_MAX, 12);
    uint16_t kp_tmp = float_to_uint(kp, KP_MIN, KP_MAX, 12);
    uint16_t kd_tmp = float_to_uint(kd, KD_MIN, KD_MAX, 12);
    uint16_t tor_tmp = float_to_uint(torque, T_MIN, T_MAX, 12);
    tx_data[0] = (pos_tmp >> 8);
    tx_data[1] = pos_tmp;
    tx_data[2] = (vel_tmp >> 4);
    tx_data[3] = ((vel_tmp&0xF)<<4)|(kp_tmp>>8);
    tx_data[4] = kp_tmp;
    tx_data[5] = (kd_tmp >> 4);
    tx_data[6] = ((kd_tmp&0xF)<<4)|(tor_tmp>>8);
    tx_data[7] = tor_tmp;
    this->hcan->SendMessages(this->can_id, tx_data, 8, false);
}

void DM4310::Pos_Speed_Msg_Send(float pos, float vel) const
{
    if (this->mode !=POSITION_AND_SPEED_MODE)
    {
        DM_Error_Handler();
        return;
    }
    const auto pbuf = reinterpret_cast<uint8_t*>(&pos);
    const auto vbuf = reinterpret_cast<uint8_t*>(&vel);
    uint8_t tx_data[8];
    tx_data[0] = *pbuf;
    tx_data[1] = *(pbuf+1);
    tx_data[2] = *(pbuf+2);
    tx_data[3] = *(pbuf+3);
    tx_data[4] = *vbuf;
    tx_data[5] = *(vbuf+1);
    tx_data[6] = *(vbuf+2);
    tx_data[7] = *(vbuf+3);
    this->hcan->SendMessages(this->can_id + 0x100, tx_data, 8, false);

}

void DM4310::Speed_Msg_Send(float vel) const
{
    if (this->mode !=SPEED_MODE)
    {
        DM_Error_Handler();
        return;
    }
    const auto vbuf = reinterpret_cast<uint8_t*>(&vel);
    uint8_t tx_data[4];
    uint32_t tx_mailbox;
    tx_data[0] = *vbuf;
    tx_data[1] = *(vbuf+1);
    tx_data[2] = *(vbuf+2);
    tx_data[3] = *(vbuf+3);
    this->hcan->SendMessages(this->can_id + 0x200, tx_data, 4, false);


}

void DM4310::Deserialize_Status(const uint8_t* status_buffer)
{

    int p_int=(status_buffer[1]<<8)|status_buffer[2];
    int v_int=(status_buffer[3]<<4)|(status_buffer[4]>>4);
    int t_int=((status_buffer[4]&0xF)<<8)|status_buffer[5];
    this->status.Position = uint_to_float(p_int, P_MIN, P_MAX, 16); // (-12.5,12.5)
    this->status.Speed = uint_to_float(v_int, V_MIN, V_MAX, 12); // (-45.0,45.0)
    this->status.Torque= uint_to_float(t_int, T_MIN, T_MAX, 12); // (-18.0,18.0)
    this->status.error_code = status_buffer[0]>>4;

    this->status.Temperature_MOS = status_buffer[6];
    this->status.Temperature_Rotor = status_buffer[7];

}

void DM4310::Enable() const
{   
    uint8_t pcan_id;
    switch(this->mode)
    {
        case(MIT_MODE):
        case(MIT_TORQUE_MODE):
            pcan_id = this->can_id;
            break;
        case(POSITION_AND_SPEED_MODE):
            pcan_id = this->can_id+0x100;
            break;
        case(SPEED_MODE):
            pcan_id = this->can_id+0x200;
            break;
        case(NONE_MODE):
            DM_Error_Handler();
            break;
    }
    uint8_t tx_data[8];
    tx_data[0] = 0xFF;
    tx_data[1] = 0xFF;
    tx_data[2] = 0xFF;
    tx_data[3] = 0xFF;
    tx_data[4] = 0xFF;
    tx_data[5] = 0xFF;
    tx_data[6] = 0xFF;
    tx_data[7] = 0xFC;
    this->hcan->SendMessages(pcan_id, tx_data, 8, false);

}

void DM4310::Disable() const
{
    uint8_t pcan_id;
    switch(this->mode)
    {
        case(MIT_MODE):
        case(MIT_TORQUE_MODE):
            pcan_id = this->can_id;
            break;
        case(POSITION_AND_SPEED_MODE):
            pcan_id = this->can_id+0x100;
            break;
        case(SPEED_MODE):
            pcan_id = this->can_id+0x200;
            break;
        case(NONE_MODE):
            DM_Error_Handler();
            break;
    }
    uint8_t tx_data[8];
    tx_data[0] = 0xFF;
    tx_data[1] = 0xFF;
    tx_data[2] = 0xFF;
    tx_data[3] = 0xFF;
    tx_data[4] = 0xFF;
    tx_data[5] = 0xFF;
    tx_data[6] = 0xFF;
    tx_data[7] = 0xFD;
    this->hcan->SendMessages(pcan_id, tx_data, 8, false);
}

void DM4310::Reset_Error() const
{
    uint8_t pcan_id;
    switch(this->mode)
    {
        case(MIT_MODE):
        case(MIT_TORQUE_MODE):
            pcan_id = this->can_id;
            break;
        case(POSITION_AND_SPEED_MODE):
            pcan_id = this->can_id+0x100;
            break;
        case(SPEED_MODE):
            pcan_id = this->can_id+0x200;
            break;
        case(NONE_MODE):
            DM_Error_Handler();
            break;
    }
    uint8_t tx_data[8];
    tx_data[0] = 0xFF;
    tx_data[1] = 0xFF;
    tx_data[2] = 0xFF;
    tx_data[3] = 0xFF;
    tx_data[4] = 0xFF;
    tx_data[5] = 0xFF;
    tx_data[6] = 0xFF;
    tx_data[7] = 0xFB;
    this->hcan->SendMessages(pcan_id, tx_data, 8, false);
}


void DM4310::Pid_Update(float target)
{
    if (this->mode!=MIT_TORQUE_MODE)
    {
        DM_Error_Handler();
        return;
    }
    if (this->pid_type == POSITION_LOOP)
    {
        float target_pos = (target - P_MIN) * (2 * M_PI) / (P_MAX - P_MIN);
        float current_pos = (this->status.Position - P_MIN) * (2 * M_PI) / (P_MAX - P_MIN);
        this->pid_angle.Pid_Update(current_pos ,  target_pos);
        this->pid_speed.Pid_Update(this->status.Speed , this->pid_angle.Get_Output());
        MIT_Ctl_Msg_Send(0,0,0,0,this->pid_speed.Get_Output());

    }
    else if (this->pid_type == SPEED_LOOP)
    {
        this->pid_speed.Pid_Update(this->status.Speed,target);
        MIT_Ctl_Msg_Send(0,0,0,0,this->pid_speed.Get_Output());
    }
    else
    {
        DM_Error_Handler();
    }
}

void DM4310::DM_Error_Handler()
{
    std::cout<<"DM4310 Error: Something wrong happened"<<std::endl;
}   

void DM4310::Set_Can_ID(uint8_t can_id)
{
    this->can_id = can_id;
}
void DM4310::Set_Master_ID(uint8_t master_id)
{
    this->master_id = master_id;
}

void DM4310::Set_CTL_Mode(Motor_DM_Mode mode)
{
    this->mode = mode;
}

// void DM4310::Bind_CAN(PCAN* hcan)
// {
//     this->hcan = hcan;
// }

void DM4310::Set_Pid_Type(const Pid_Type pid_type)
{
    this->pid_type = pid_type;
    if (this->pid_type == POSITION_LOOP)
    {
        this->pid_angle.Set_Parameters(0.0,0.0,0,0.001);
        this->pid_speed.Set_Parameters(0,0,0,0.001);
    }
    else if (this->pid_type == SPEED_LOOP)
    {
        this->pid_angle.Set_Parameters(0,0,0,0.001);
        this->pid_speed.Set_Parameters(0,0,0,0.001);
    }
}

uint8_t DM4310::Get_Can_ID() const
{
    return this->can_id;
}

uint8_t DM4310::Get_Master_ID() const
{
    return this->master_id;
}


bool DM4310::Handle_CAN_Message(const CANMessageData& message)
{
    if ((this->mode == MIT_MODE || this->mode == MIT_TORQUE_MODE) && message.id == this->can_id )
    {   
        Deserialize_Status(message.data);
        return true;
    }
    else if ( this->mode == POSITION_AND_SPEED_MODE && message.id == this->can_id + 0x100  )
    {
        Deserialize_Status(message.data);
        return true;
    }
    else if (this->mode == SPEED_MODE &&  message.id == this->can_id + 0x200  )
    {
        Deserialize_Status(message.data);
        return true;
    }
    return false;
}