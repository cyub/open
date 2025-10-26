from langchain.chat_models import init_chat_model
from langchain_core.output_parsers import JsonOutputParser
from pydantic import BaseModel, Field
from langchain_core.prompts import PromptTemplate
from langchain_core.runnables import RunnableLambda
import os

api_key = os.environ["SILICONFLOW_API_KEY"]

model = init_chat_model(
    model="Qwen/Qwen3-8B",
    model_provider="openai",
    base_url="https://api.siliconflow.cn/v1/",  # 硅基流动模型的请求url
    api_key=api_key,
)

news_gen_prompt = PromptTemplate.from_template(
    "请根据以下新闻标题撰写一段简短的新闻内容（100字以内）：\n\n标题：{title}"
)

new_chain = news_gen_prompt | model


class Summary(BaseModel):
    time: str = Field(description="事件发生的时间")
    location: str = Field(description="事件发生的地点")
    event: str = Field(description="发生的具体事件")


parser = JsonOutputParser(pydantic_object=Summary)
summary_prompt = PromptTemplate(
    template="请从下面这段新闻内容中提取关键信息，并返回结构化JSON格式：\n\n{news}\n\n{format_instructions}",
    input_variables=["news"],
    partial_variables={"format_instructions": parser.get_format_instructions()},
)
summary_chain = summary_prompt | model | parser


def debug_print(x):
    print("中间结果：", x)
    return x


debug_node = RunnableLambda(debug_print)
full_chain = new_chain | debug_node | summary_chain
result = full_chain.invoke({"title": "苹果公司在加州发布新款AI芯片"})

print(result)
