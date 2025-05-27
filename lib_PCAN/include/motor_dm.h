#ifndef MOTOR_DM_H
#define MOTOR_DM_H

#include "motor.h"

#define P_MAX (3.141593)
#define P_MIN (-3.141593)
#define V_MAX (30)
#define V_MIN (-30)
#define KP_MAX (500)
#define KP_MIN (0)
#define KD_MAX (5)
#define KD_MIN (0)
#define T_MAX (10)
#define T_MIN (-10)


class DM4310 final : private Motor
{
public:
    explicit DM4310( PCAN* hcan  ,Motor_DM_Mode mode = NONE_MODE, uint8_t can_id = 0, uint8_t master_id = 0 , Pid_Type pid_type = NONE_PID);
    void Set_Angle(float angle , float speed ) override;
    void Set_Speed(float speed) override;


    void Enable() const;
    void Disable() const;
    void Reset_Error() const;
    void Deserialize_Status(const uint8_t* status_buffer);
    void MIT_Ctl_Msg_Send(float pos , float vel , float kp , float kd , float torque) const;
    void Pos_Speed_Msg_Send(float pos , float vel) const;
    void Speed_Msg_Send(float vel) const;

    void Bind_CAN(PCAN* hcan);
    uint8_t Get_Can_ID() const;
    uint8_t Get_Master_ID() const;
    void Set_CTL_Mode(Motor_DM_Mode mode);
    void Set_Can_ID(uint8_t can_id);
    void Set_Master_ID(uint8_t master_id);
    void Set_Pid_Type(Pid_Type pid_type) override;

    PID pid_speed ;
    PID pid_angle ;
    uint8_t status_buffer[8] = {0};
private:
    static void DM_Error_Handler();
    void Pid_Update(float target) override;

    DM_Motor_Status status;
    uint8_t master_id;
    uint8_t can_id;
    Motor_DM_Mode mode;

    //继承来的私有变量
    Pid_Type pid_type;
    PCAN* hcan = nullptr;

};

 inline float uint_to_float(const int x_int , const float x_min , const float x_max , const int bits)
{
    /// converts unsigned int to float, given range and number of bits ///
    const float span = x_max - x_min;
    const float offset = x_min;
    return (static_cast<float>(x_int)*span/static_cast<float>((1 << bits) - 1)) + offset;
}

inline int float_to_uint(const float x, const float x_min, const float x_max, const int bits){
    /// Converts a float to an unsigned int, given range and number of bits
    const float span = x_max - x_min;
    const float offset = x_min;
    return static_cast<int>((x - offset) * static_cast<float>((1 << bits) - 1) / span);
    }


#endif //MOTOR_DM_H
