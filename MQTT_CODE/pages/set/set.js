const app = getApp();

const {
  connect
} = require("../../utils/mqtt4/mqtt");
const mqttHost = "broker.emqx.io"; //mqtt 服务器域名/IP
const mqttPort = 8084; //mqtt 服务器域名/IP

const deviceSubTopic = "/solitary/sub"; //  设备订阅topic（小程序发布命令topic）
const devicePubTopic = "/solitary/pub"; //  设备发布topic（小程序订阅数据topic）
const mpSubTopic = devicePubTopic;
const mpPubTopic = deviceSubTopic;

const mqttUrl = `wxs://${mqttHost}:${mqttPort}/mqtt`; //  mqtt连接路径
Page({
  data: {
    speed:'',
    client: null,
    online: null,
    smoke: 0,
    LAX: null,
    TRCT1: null,
    TRCT2: null,
    TRCT3: null,
    TRCT4: null,
    Led: false,
    Beep: '',
    Light: false,
    switch1Checked: false,
    switch2Checked: false,
    switch3Checked: false,
    switch4Checked: false,
    switch5Checked: false,
  },

  onLedChange(event) {
    const that = this
    console.log(event.detail.value);
    const sw = event.detail.value
    that.setData({
      Led: sw
    })

    if (sw) {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "LED",
        value: 0
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——开灯');
        }
      })
    } else {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "LED",
        value: 1
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——关灯');
        }
      })
    }

  },
  onBeepChange(event) {
    const that = this;
    console.log(event.detail.value);
    const sw = event.detail.value;
    that.setData({
      Beep: sw
    })
    if (sw) {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "Beep",
        value: 1
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——开启报警器');
        }
      })
    } else {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "Beep",
        value: 0
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——关闭报警器');
        }
      })
    }
  },
  switch1Change(event) {
    const that = this
    console.log(event.detail.value);
    const sw = event.detail.value
    that.setData({
      switch1Checked: sw
    })

    if (sw) {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "switch1Checked",
        value: 1
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——开启干垃圾桶盖');
        }
      })
    } else {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "switch1Checked",
        value: 0
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——关闭干垃圾桶盖');
        }
      })
    }

  },
  switch2Change(event) {
    const that = this
    console.log(event.detail.value);
    const sw = event.detail.value
    that.setData({
      switch2Checked: sw
    })

    if (sw) {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "switch2Checked",
        value: 1
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——开启湿垃圾桶盖');
        }
      })
    } else {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "switch2Checked",
        value: 0
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——关闭湿垃圾桶盖');
        }
      })
    }

  },
  switch3Change(event) {
    const that = this
    console.log(event.detail.value);
    const sw = event.detail.value
    that.setData({
      switch3Checked: sw
    })

    if (sw) {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "switch3Checked",
        value: 1
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——开启有害垃圾桶盖');
        }
      })
    } else {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "switch3Checked",
        value: 0
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——关闭有害垃圾桶盖');
        }
      })
    }

  },
  switch4Change(event) {
    const that = this
    console.log(event.detail.value);
    const sw = event.detail.value
    that.setData({
      switch4Checked: sw
    })

    if (sw) {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "switch4Checked",
        value: 1
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——开启可回收物桶盖');
        }
      })
    } else {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "switch4Checked",
        value: 0
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——关闭可回收物桶盖');
        }
      })
    }

  },
  switch5Change(event) {
    const that = this
    console.log(event.detail.value);
    const sw = event.detail.value
    that.setData({
      switch5Checked: sw
    })

    if (sw) {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "switch5Checked",
        value: 1
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——开启消毒');
        }
      })
    } else {
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target: "switch5Checked",
        value: 0
      }), function (err) {
        if (!err) {
          console.log('成功下发指令——关闭消毒');
        }
      })
    }

  },
  speedChange(e) {
    const that = this
    console.log(e.detail.value);
    const sw = e.detail.value
    that.setData({speed:sw})
    if(1){
      that.data.client.publish(mpPubTopic,JSON.stringify({
        "target":"speed",
        "value":e.detail.value
      }),function (err){
        if(!err)
        {
          console.log('成功下发指令-speed');
        }
      })
    }
  },
  // 打印连接mqtt服务器
  onShow() {
    const that = this
    that.setData({
      client: connect(`wxs://${mqttHost}:${mqttPort}/mqtt`)
    })

    that.data.client.on('connect', function (params) {
      console.log('成功连接到MQTT服务器！');
      wx.showToast({
        title: '连接成功',
        icon: 'success',
        mask: true
      })

      that.data.client.subscribe(mpSubTopic, function (err) {
        if (!err) {
          console.log('成功订阅设备上行数据Topic！')
        }
      })

    })

    that.data.client.on('message', function (topic, message) {
      console.log(topic);
      console.log(message);
      //message 是 16进制的buffer字节流

      let dataFromDev = {}
      try {
        dataFromDev = JSON.parse(message)
        console.log(dataFromDev);
        that.setData({
          smoke: dataFromDev.smoke,
          LAX: dataFromDev.LAX,
          TRCT1: dataFromDev.TRCT1,
          TRCT2: dataFromDev.TRCT2,
          TRCT3: dataFromDev.TRCT3,
          TRCT4: dataFromDev.TRCT4,
          online: true,
        })

      } catch (error) {
        console.log('JSON解析失败', error);
      }

    })

  }
})