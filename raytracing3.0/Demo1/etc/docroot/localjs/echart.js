// 基于准备好的dom，初始化echarts实例
var button1 = document.getElementById("pathloss");
var button2 = document.getElementById("powerdelay");
var button3 = document.getElementById("meandelay");
var button4 = document.getElementById("dimpowerdelay");

var myChart = echarts.init(document.getElementById("main"));
button4.onclick = function (ev) {
    loadDimenPowerDelay();
};
button3.onclick = function (ev) {
    console.log("b3");
    loadbutton2();
};



function loadbutton2() {
    option = {
        title: {
            text: '平均附加时延',
            subtext: '附加时延'
        },
        tooltip: {},
        backgroundColor: '#fff',
        visualMap: {
            show: false,
            dimension: 2,
            min: -1,
            max: 1,
            inRange: {
                color: ['#313695', '#4575b4', '#74add1', '#abd9e9', '#e0f3f8', '#ffffbf', '#fee090', '#fdae61', '#f46d43', '#d73027', '#a50026']
            }
        },
        xAxis3D: {
            type: 'value'
        },
        yAxis3D: {
            type: 'value'
        },
        zAxis3D: {
            type: 'value'
        },
        grid3D: {
            viewControl: {
                // projection: 'orthographic'
            }
        },
        series: [{
            type: 'surface',
            wireframe: {
                // show: false
            },
            equation: {
                x: {
                    step: 0.05
                },
                y: {
                    step: 0.05
                },
                z: function (x, y) {
                    if (Math.abs(x) < 0.1 && Math.abs(y) < 0.1) {
                        return '-';
                    }
                    return Math.sin(x * Math.PI) * Math.sin(y * Math.PI);
                }
            }
        }]
    };
    myChart.setOption(option);
}

function loadDimenPowerDelay() {
    myChart.setOption({
        title: {
            text: '功率时延空间分布',
            subtext: '功率时延空间分布'
        },
        grid3D: {},
        xAxis3D: {},
        yAxis3D: {},
        zAxis3D: {},
        series: [{
            type: 'scatter3D',
            symbolSize: 1,
            data: [
                [-1, -1, -1],
                [0, 0, 0],
                [1, 1, 1]
            ],
            itemStyle: {
                opacity: 1
            }
        }]
    });
}

function loadPathLoss(pathLossOrder, pathLoss) {
    // 指定图表的配置项和数据
    option = {
        title: {
            text: 'Ray-Tracing 路径损耗',
            subtext: '路径损耗分布'
        },
        tooltip: {
            trigger: 'axis'
        },
        legend: {
            data: ['路径损耗']
        },
        toolbox: {
            show: true,
            feature: {
                mark: {
                    show: true
                },
                dataView: {
                    show: true,
                    readOnly: false
                },
                magicType: {
                    show: true,
                    type: ['line', 'bar']
                },
                restore: {
                    show: true
                },
                saveAsImage: {
                    show: true
                }
            }
        },
        calculable: true,
        xAxis: [{
            type: 'category',
            data: pathLossOrder
        }],
        yAxis: [{
            type: 'value'
        }],
        series: [{
            name: '路径损耗/dB',
            type: 'bar',
            data: pathLoss,
            markPoint: {
                data: [{
                    type: 'max',
                    name: '最大值'
                }, {
                    type: 'min',
                    name: '最小值'
                }]
            },
            markLine: {
                data: [{
                    type: 'average',
                    name: '平均值'
                }]
            }
        }]
    };
    // 使用刚指定的配置项和数据显示图表。
    myChart.setOption(option);
}