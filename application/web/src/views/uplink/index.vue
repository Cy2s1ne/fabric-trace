<template>
  <div class="uplink-container">
    <div style="color:#909399;margin-bottom: 30px">
      当前用户：{{ name }};
      用户角色: {{ userType }}
    </div>
    <div>
      <el-form ref="form" :model="tracedata" label-width="80px" size="mini" style="">
        <el-form-item v-show="userType!='管理员'&userType!='用户'" label="溯源码:" style="width: 300px" label-width="120px">
          <el-input v-model="tracedata.traceability_code" />
        </el-form-item>

        <div v-show="userType=='管理员'">
          <el-form-item label="物联网节点名称:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Farmer_input.Fa_fruitName" />
          </el-form-item>
          <el-form-item label="设备ID:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Farmer_input.Fa_origin" />
          </el-form-item>
          <el-form-item label="所有者ID:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Farmer_input.Fa_plantTime" />
          </el-form-item>
          <el-form-item label="传感器类型:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Farmer_input.Fa_pickingTime" />
          </el-form-item>
          <el-form-item label="消息内容:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Farmer_input.Fa_farmerName" />
          </el-form-item>
        </div>
        <!-- <div v-show="userType=='工厂'">
          <el-form-item label="商品名称:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Factory_input.Fac_productName" />
          </el-form-item>
          <el-form-item label="生产批次:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Factory_input.Fac_productionbatch" />
          </el-form-item>
          <el-form-item label="生产时间:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Factory_input.Fac_productionTime" />
          </el-form-item>
          <el-form-item label="工厂名称与厂址:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Factory_input.Fac_factoryName" />
          </el-form-item>
          <el-form-item label="工厂电话:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Factory_input.Fac_contactNumber" />
          </el-form-item>
        </div>
        <div v-show="userType=='运输司机'">
          <el-form-item label="姓名:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Driver_input.Dr_name" />
          </el-form-item>
          <el-form-item label="年龄:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Driver_input.Dr_age" />
          </el-form-item>
          <el-form-item label="联系电话:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Driver_input.Dr_phone" />
          </el-form-item>
          <el-form-item label="车牌号:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Driver_input.Dr_carNumber" />
          </el-form-item>
          <el-form-item label="运输记录：" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Driver_input.Dr_transport" />
          </el-form-item>
        </div>
        <div v-show="userType=='商店'">
          <el-form-item label="存入时间:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Shop_input.Sh_storeTime" />
          </el-form-item>
          <el-form-item label="销售时间:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Shop_input.Sh_sellTime" />
          </el-form-item>
          <el-form-item label="商店名称:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Shop_input.Sh_shopName" />
          </el-form-item>
          <el-form-item label="商店位置:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Shop_input.Sh_shopAddress" />
          </el-form-item>
          <el-form-item label="商店电话:" style="width: 300px" label-width="120px">
            <el-input v-model="tracedata.Shop_input.Sh_shopPhone" />
          </el-form-item>
        </div> -->
      </el-form>
      <span slot="footer" style="color: gray;" class="dialog-footer">
        <el-button v-show="userType != '用户'" type="primary" plain style="margin-left: 220px;" @click="submittracedata()">提 交</el-button>
      </span>
      <span v-show="userType == '用户'" slot="footer" style="color: gray;" class="dialog-footer">
        用户没有权限录入！请使用溯源功能!
      </span>
    </div>

    <!-- 添加MQTT连接表单 -->
    <el-form ref="mqttForm" :model="mqttConfig" label-width="120px" size="mini" style="margin-bottom: 20px">
      <el-form-item label="MQTT服务器:" style="width: 300px">
        <el-input v-model="mqttConfig.broker" placeholder="ws://175.24228.169:8084" />
      </el-form-item>
      <el-form-item label="主题:" style="width: 300px">
        <el-input v-model="mqttConfig.topic" placeholder="sensor/data" />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" :loading="mqttLoading" @click="connectMqtt">
          {{ mqttConnected ? '断开连接' : '连接' }}
        </el-button>
      </el-form-item>
    </el-form>

    <!-- 添加消息显示区域 -->
    <div v-if="mqttConnected" style="margin-top: 20px">
      <h3>接收到的消息</h3>
      <el-card v-if="mqttMessages.length > 0" style="margin-top: 10px">
        <div v-for="(msg, index) in mqttMessages" :key="index" style="margin-bottom: 10px">
          <div style="display: flex; justify-content: space-between">
            <span style="color: #409EFF">主题: {{ msg.topic }}</span>
            <span style="color: #909399">{{ msg.timestamp }}</span>
          </div>
          <div style="margin-top: 5px; word-break: break-all">
            消息内容: {{ msg.message }}
          </div>
          <el-divider v-if="index !== mqttMessages.length - 1"></el-divider>
        </div>
      </el-card>
      <el-empty v-else description="暂无消息"></el-empty>
      <!-- 添加清空按钮 -->
      <el-button
        v-if="mqttMessages.length > 0"
        type="danger"
        size="small"
        style="margin-top: 10px"
        @click="clearMessages"
      >
        清空消息
      </el-button>
    </div>
  </div>
</template>

<script>
import { mapGetters } from 'vuex'
import { uplink } from '@/api/trace'
import mqtt from '@/utils/mqtt.js'
export default {
  name: 'Uplink',
  data() {
    return {
      tracedata: {
        traceability_code: '',
        Farmer_input: {
          Fa_fruitName: '',
          Fa_origin: '',
          Fa_plantTime: '',
          Fa_pickingTime: '',
          Fa_farmerName: ''
        },
        Factory_input: {
          Fac_productName: '',
          Fac_productionbatch: '',
          Fac_productionTime: '',
          Fac_factoryName: '',
          Fac_contactNumber: ''
        },
        Driver_input: {
          Dr_name: '',
          Dr_age: '',
          Dr_phone: '',
          Dr_carNumber: '',
          Dr_transport: ''
        },
        Shop_input: {
          Sh_storeTime: '',
          Sh_sellTime: '',
          Sh_shopName: '',
          Sh_shopAddress: '',
          Sh_shopPhone: ''
        }
      },
      loading: false,
      mqttConfig: {
        broker: 'ws://175.24.228.169:8083/mqtt',
        topic: 'blockchain/error'
      },
      mqttClient: null,
      mqttConnected: false,
      mqttLoading: false,
      mqttMessages: [],
      maxMessages: 50
    }
  },
  computed: {
    ...mapGetters([
      'name',
      'userType'
    ])
  },
  beforeDestroy() {
    if (this.mqttClient) {
      this.mqttClient.end()
    }
    this.mqttMessages = []
  },
  methods: {
    submittracedata() {
      if (this.loading) {
        return // 防止重复提交
      }

      console.log(this.tracedata)
      const loading = this.$loading({
        lock: true,
        text: '数据上链中...',
        spinner: 'el-icon-loading',
        background: 'rgba(0, 0, 0, 0.7)'
      })
      this.loading = true

      var formData = new FormData()
      formData.append('traceability_code', this.tracedata.traceability_code)

      // 根据不同的用户给arg1、arg2、arg3..赋值
      switch (this.userType) {
        case '管理员':
          formData.append('arg1', this.tracedata.Farmer_input.Fa_fruitName)
          formData.append('arg2', this.tracedata.Farmer_input.Fa_origin)
          formData.append('arg3', this.tracedata.Farmer_input.Fa_plantTime)
          formData.append('arg4', this.tracedata.Farmer_input.Fa_pickingTime)
          formData.append('arg5', this.tracedata.Farmer_input.Fa_farmerName)
          break
        case '工厂':
          formData.append('arg1', this.tracedata.Factory_input.Fac_productName)
          formData.append('arg2', this.tracedata.Factory_input.Fac_productionbatch)
          formData.append('arg3', this.tracedata.Factory_input.Fac_productionTime)
          formData.append('arg4', this.tracedata.Factory_input.Fac_factoryName)
          formData.append('arg5', this.tracedata.Factory_input.Fac_contactNumber)
          break
        case '运输司机':
          formData.append('arg1', this.tracedata.Driver_input.Dr_name)
          formData.append('arg2', this.tracedata.Driver_input.Dr_age)
          formData.append('arg3', this.tracedata.Driver_input.Dr_phone)
          formData.append('arg4', this.tracedata.Driver_input.Dr_carNumber)
          formData.append('arg5', this.tracedata.Driver_input.Dr_transport)
          break
        case '商店':
          formData.append('arg1', this.tracedata.Shop_input.Sh_storeTime)
          formData.append('arg2', this.tracedata.Shop_input.Sh_sellTime)
          formData.append('arg3', this.tracedata.Shop_input.Sh_shopName)
          formData.append('arg4', this.tracedata.Shop_input.Sh_shopAddress)
          formData.append('arg5', this.tracedata.Shop_input.Sh_shopPhone)
          break
      }

      uplink(formData).then(res => {
        this.loading = false
        if (res.code === 200) {
          loading.close()
          this.$message({
            message: '上链成功，交易ID：' + res.txid + '\n溯源码：' + res.traceability_code,
            type: 'success'
          })
        } else {
          loading.close()
          this.$message({
            message: '上链失败',
            type: 'error'
          })
        }
      }).catch(err => {
        this.loading = false
        loading.close()
        console.log(err)
        this.$message.error('上链过程中发生错误')
      })
    },
    async connectMqtt() {
      if (this.mqttConnected) {
        try {
          await this.mqttClient.end()
          this.mqttClient = null
          this.mqttConnected = false
          this.mqttLoading = false
          this.$message.success('已断开连接')
        } catch (error) {
          console.error('Disconnect error:', error)
          this.$message.error('断开连接失败')
        }
        return
      }

      if (!this.mqttConfig.broker || !this.mqttConfig.topic) {
        this.$message.error('请填写MQTT服务器地址和主题')
        return
      }

      if (!this.mqttConfig.broker.startsWith('ws://') && !this.mqttConfig.broker.startsWith('wss://')) {
        this.$message.error('MQTT服务器地址必须以 ws:// 或 wss:// 开头')
        return
      }

      this.mqttLoading = true
      try {
        console.log('正在连接到:', this.mqttConfig.broker)
        const options = {
          keepalive: 60,
          clientId: 'mqttjs_' + Math.random().toString(16).substr(2, 8),
          protocolVersion: 4,
          clean: true,
          reconnectPeriod: 1000,
          connectTimeout: 30 * 1000,
          rejectUnauthorized: false
        }

        this.mqttClient = mqtt.connect(this.mqttConfig.broker, options)

        this.mqttClient.on('connecting', () => {
          console.log('正在连接到MQTT服务器...')
        })

        this.mqttClient.on('connect', () => {
          console.log('MQTT连接成功')
          this.mqttConnected = true
          this.mqttLoading = false
          this.$message.success('MQTT 连接成功')
          this.mqttClient.subscribe(this.mqttConfig.topic, (err) => {
            if (err) {
              console.error('主题订阅失败:', err)
              this.$message.error(`主题订阅失败: ${err.message}`)
              return
            }
            console.log('成功订阅主题:', this.mqttConfig.topic)
            this.$message.success('主题订阅成功')
          })
        })

        this.mqttClient.on('message', (topic, message) => {
          try {
            const messageStr = message.toString()
            console.log('收到消息:', topic, messageStr)
            // 添加新消息到消息列表
            this.mqttMessages.unshift({
              topic,
              message: messageStr,
              timestamp: new Date().toLocaleString()
            })
            // 限制消息数量
            if (this.mqttMessages.length > this.maxMessages) {
              this.mqttMessages = this.mqttMessages.slice(0, this.maxMessages)
            }

            // 尝试解析消息为JSON并填充表单
            try {
              const data = JSON.parse(messageStr)
              if (this.userType === '管理员') {
                // 填充管理员表单
                this.tracedata.Farmer_input = {
                  Fa_fruitName: data.nodeName || '',
                  Fa_origin: data.deviceId || '',
                  Fa_plantTime: data.ownerId || '',
                  Fa_pickingTime: data.sensorType || '',
                  Fa_farmerName: data.message || ''
                }
                // 自动生成溯源码（可以根据需要修改生成逻辑
                this.tracedata.traceability_code = 'TC_' + new Date().getTime()
                // 自动提交表单
                this.$nextTick(() => {
                  this.submittracedata()
                })
              }
            } catch (parseError) {
              console.error('JSON解析错误:', parseError)
              this.$message.warning('消息格式不正确，无法自动填充表单')
            }
          } catch (error) {
            console.error('处理消息时出错:', error)
          }
        })

        this.mqttClient.on('error', (error) => {
          console.error('MQTT错误:', error)
          this.$message.error(`MQTT连接错误: ${error.message}`)
          this.mqttLoading = false
          this.mqttConnected = false
        })

        this.mqttClient.on('close', () => {
          console.log('MQTT连接关闭')
          this.mqttConnected = false
          this.mqttLoading = false
        })

        this.mqttClient.on('offline', () => {
          console.log('MQTT客户端离线')
          this.mqttConnected = false
        })
      } catch (error) {
        console.error('连接错误:', error)
        this.$message.error(`连接失败: ${error.message}`)
        this.mqttLoading = false
      }
    },
    clearMessages() {
      this.$confirm('确定要清空所有消息吗?', '提示', {
        confirmButtonText: '确定',
        cancelButtonText: '取消',
        type: 'warning'
      }).then(() => {
        this.mqttMessages = []
        this.$message({
          type: 'success',
          message: '消息已清空'
        })
      }).catch(() => {
        // 取消清空操作
      })
    }
  }
}

</script>

<style lang="scss" scoped>
.uplink {
  &-container {
    margin: 30px;
  }
  &-text {
    font-size: 30px;
    line-height: 46px;
  }
}

.el-card {
  margin-top: 20px;
}

.message-item {
  padding: 10px 0;
  border-bottom: 1px solid #EBEEF5;
  &:last-child {
    border-bottom: none;
  }
}
</style>
