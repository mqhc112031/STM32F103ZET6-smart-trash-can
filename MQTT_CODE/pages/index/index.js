const app=getApp();

const { connect } = require("../../utils/mqtt4/mqtt");
const mqttHost = "broker.emqx.io"; //mqtt 服务器域名/IP
const mqttPort = 8084; //mqtt 服务器域名/IP

const deviceSubTopic = "/solitary/sub"; //  设备订阅topic（小程序发布命令topic）
const devicePubTopic = "/solitary/pub"; //  设备发布topic（小程序订阅数据topic）
const mpSubTopic = devicePubTopic;
const mpPubTopic = deviceSubTopic;

const mqttUrl = `wxs://${mqttHost}:${mqttPort}/mqtt`; //  mqtt连接路径

Page({
    data: {
      client: null,
      airText: '请求中',//空气质量
      airValue: 0,//空气指数
      weather: '请求中',//天气
      weatherAdvice: '请求中'//天气建议
    },

  //获取定位
  // wx.request({
  //   url: `https://geoapi.heweather.net/v2/city/lookup?location=113.87,22.90&key=${key}`, //获取位置信息
  //   success: (res) => {
  //     const { adm2, name } = res.data.location[0]
  //     this.setData({
  //       area: name,
  //       city: adm2
  //     })
  //   }
  // })

    onLoad() {
      var that = this
      const key = 'aeef447009d34235b5c7da9af2b12025'

      wx.request({
        url: `https://devapi.heweather.net/v7/weather/now?location=113.87,22.90&key=${key}`, //获取天气数据API接口
        success: (res) => {
          this.setData({ weather: res.data.now.text })
        }
      })

      wx.request({
        url: `https://devapi.heweather.net/v7/air/now?location=113.87,22.90&key=${key}`, //
        success: res => {
          let { aqi, category } = res.data.now
          this.setData({
            airText: category,
            airValue: aqi
          })
        }
      })

      wx.request({
        url: `https://devapi.heweather.net/v7/indices/1d?type=6&location=113.87,22.90&key=${key}`, //获取位置信息
        success: res => {
          this.setData({
            weatherAdvice: res.data.daily[0].text
          })
        }
      })
    },
  
    
    // 打印连接mqtt服务器
    onShow(){
      const that = this
      that.setData({
         client:connect(`wxs://${mqttHost}:${mqttPort}/mqtt`)
      })
  
      that.data.client.on('connect',function (params){
        console.log('成功连接到MQTT服务器！');
        wx.showToast({
          title: '连接成功',
          icon:'success',
          mask:true
        })
  
          that.data.client.subscribe(mpSubTopic,function (err){
            if(!err){
              console.log('成功订阅设备上行数据Topic！')
            }
          })
  
      })
  
  
    } 
})